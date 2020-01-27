import sys

print 'Debug enabled. Debug info written to debug.out'
debug = open('debug.out','w')
def trace_calls(frame, event, arg,output=debug):
    if event != 'call':
        return
    co = frame.f_code
    func_name = co.co_name
    if func_name == 'write':
        # Ignore write() calls from print statements
        return
    func_line_no = frame.f_lineno
    func_filename = co.co_filename
    caller = frame.f_back
    caller_line_no = caller.f_lineno
    caller_filename = caller.f_code.co_filename
    output.write('Call to %s on line %s of %s from line %s of %s\n' %
                 (func_name, func_line_no, func_filename,
                  caller_line_no, caller_filename))
    return

sys.settrace(trace_calls)
