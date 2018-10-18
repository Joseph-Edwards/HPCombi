"""util.py - General utilities for running, loading, and processing benchmarks
"""
import json
import os
import tempfile
import subprocess
import sys
import re
import copy

# Input file type enumeration
IT_Invalid    = 0
IT_JSON       = 1
IT_Executable = 2

_num_magic_bytes = 2 if sys.platform.startswith('win') else 4
def is_executable_file(filename):
    """
    Return 'True' if 'filename' names a valid file which is likely
    an executable. A file is considered an executable if it starts with the
    magic bytes for a EXE, Mach O, or ELF file.
    """
    if not os.path.isfile(filename):
        return False
    with open(filename, mode='rb') as f:
        magic_bytes = f.read(_num_magic_bytes)
    if sys.platform == 'darwin':
        return magic_bytes in [
            b'\xfe\xed\xfa\xce',  # MH_MAGIC
            b'\xce\xfa\xed\xfe',  # MH_CIGAM
            b'\xfe\xed\xfa\xcf',  # MH_MAGIC_64
            b'\xcf\xfa\xed\xfe',  # MH_CIGAM_64
            b'\xca\xfe\xba\xbe',  # FAT_MAGIC
            b'\xbe\xba\xfe\xca'   # FAT_CIGAM
        ]
    elif sys.platform.startswith('win'):
        return magic_bytes == b'MZ'
    else:
        return magic_bytes == b'\x7FELF'


def is_json_file(filename):
    """
    Returns 'True' if 'filename' names a valid JSON output file.
    'False' otherwise.
    """
    try:
        with open(filename, 'r') as f:
            json.load(f)
        return True
    except:
        pass
    return False


def classify_input_file(filename):
    """
    Return a tuple (type, msg) where 'type' specifies the classified type
    of 'filename'. If 'type' is 'IT_Invalid' then 'msg' is a human readable
    string represeting the error.
    """
    ftype = IT_Invalid
    err_msg = None
    if not os.path.exists(filename):
        err_msg = "'%s' does not exist" % filename
    elif not os.path.isfile(filename):
        err_msg = "'%s' does not name a file" % filename
    elif is_executable_file(filename):
        ftype = IT_Executable
    elif is_json_file(filename):
        ftype = IT_JSON
    else:
        err_msg = "'%s' does not name a valid benchmark executable or JSON file" % filename
    return ftype, err_msg


def check_input_file(filename):
    """
    Classify the file named by 'filename' and return the classification.
    If the file is classified as 'IT_Invalid' print an error message and exit
    the program.
    """
    ftype, msg = classify_input_file(filename)
    if ftype == IT_Invalid:
        print("Invalid input file: %s" % msg)
        sys.exit(1)
    return ftype

def find_benchmark_flag(prefix, benchmark_flags):
    """
    Search the specified list of flags for a flag matching `<prefix><arg>` and
    if it is found return the arg it specifies. If specified more than once the
    last value is returned. If the flag is not found None is returned.
    """
    assert prefix.startswith('--') and prefix.endswith('=')
    result = None
    for f in benchmark_flags:
        if f.startswith(prefix):
            result = f[len(prefix):]
    return result

def remove_benchmark_flags(prefix, benchmark_flags):
    """
    Return a new list containing the specified benchmark_flags except those
    with the specified prefix.
    """
    assert prefix.startswith('--') and prefix.endswith('=')
    return [f for f in benchmark_flags if not f.startswith(prefix)]

def load_benchmark_results(fname):
    """
    Read benchmark output from a file and return the JSON object.
    REQUIRES: 'fname' names a file containing JSON benchmark output.
    """
    with open(fname, 'r') as f:
        return json.load(f)


def run_benchmark(exe_name, benchmark_flags):
    """
    Run a benchmark specified by 'exe_name' with the specified
    'benchmark_flags'. The benchmark is run directly as a subprocess to preserve
    real time console output.
    RETURNS: A JSON object representing the benchmark output
    """
    output_name = find_benchmark_flag('--benchmark_out=',
                                      benchmark_flags)
    is_temp_output = False
    if output_name is None:
        is_temp_output = True
        thandle, output_name = tempfile.mkstemp()
        os.close(thandle)
        benchmark_flags = list(benchmark_flags) + \
                          ['--benchmark_out=%s' % output_name]

    cmd = [exe_name] + benchmark_flags
    print("RUNNING: %s" % ' '.join(cmd))
    exitCode = subprocess.call(cmd)
    if exitCode != 0:
        print('TEST FAILED...')
        sys.exit(exitCode)
    json_res = load_benchmark_results(output_name)
    if is_temp_output:
        os.unlink(output_name)
    return json_res


def run_or_load_benchmark(filename, benchmark_flags):
    """
    Get the results for a specified benchmark. If 'filename' specifies
    an executable benchmark then the results are generated by running the
    benchmark. Otherwise 'filename' must name a valid JSON output file,
    which is loaded and the result returned.
    """
    ftype = check_input_file(filename)
    if ftype == IT_JSON:
        return load_benchmark_results(filename)
    elif ftype == IT_Executable:
        return run_benchmark(filename, benchmark_flags)
    else:
        assert False # This branch is unreachable
      
def get_same_set(same, nameSets):
    newSame = set()
    if not same:
        return same
    # Get all matching word
    for exp in same :
        # \ have been added in the bench names to excape regex interpretation
        # It is necesarie to add \\ into the regex to match the \ in the names
        exp = exp.replace('\\', '\\\\\\')
        regex = re.compile(exp)
        for wordSet in nameSets:
            for word in wordSet:
                if regex.search(word):
                    newSame.add(word)

    return newSame

def get_files_set(fileName):
    path = os.path.split( os.path.abspath(fileName) )[0]
    filelist = os.listdir(path)
    regex = re.compile( fileName )
    outList = set()
    for name in filelist :
        if regex.search(name):
            outList.add(name)
    return outList
            
def get_comps_list(json1_orig, compsIn, same, nameSets):  
    comps1 = set()
    comps2 = set()

	# Generate comparisons with the regex comps arguments
    for comp in compsIn:
        try:
            a, b = comp.split('/')
        except ValueError:
            a, b = "", ""
        # \ have been added in the bench names to excape regex interpretation
        # It is necesarie to add \\ into the regex to match the \ in the names
        a = re.compile( a.replace('\\', '\\\\\\') )
        b = re.compile( b.replace('\\', '\\\\\\') )
        # TODO could do better thant nesting 3 loops
        for wordSet in nameSets[1:]:
            for word1 in wordSet: 
                if a.search(word1):
                    for word2 in wordSet:
                        if b.search(word2) and \
                            (word1 != word2) and \
                            (word2 + '/' + word1 not in comps1): # One side comparison only needed
                            comps1.add(word1 + '/' + word2)

	# Generate comparisons with the constante arguments
    # No comparison are generated for the first parameter
    # TODO could do better thant nesting 3 loops
    for wordSet in nameSets[1:]:
        if not (same & wordSet):
            for word1 in wordSet:
                for word2 in wordSet:
                    if (word1 != word2): # Do not impose one side comparison it is already done in comps1
                        comps2.add(word1 + '/' + word2)
    return comps1 & comps2

def get_regex(same, nameSets):
    expFilter = ''
    # Starts can be one of the following _ [ ^ or space
    start = '(_|^|\[| )'
    # Ends can be one of the following _ ] $ or space
    end = '(_|$|\]| )'

    expParts = set()
    # If word are in the same group regex should match one of them (logical or)
    for wordSet in nameSets:
        toAdd = same & wordSet
        if toAdd:
            expParts.add( start + '(' + '|'.join(toAdd) + ')' + end )
    if expParts:
        # If word are not in the same group regex should match all of them (logical and)
        expFilter = ''.join([ '(?=.*', (')(?=.*').join( expParts ), ')' ]) # et
    return expFilter
