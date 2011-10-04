import sys
import os
import subprocess
import tempfile

def execute(command, env=None, split_lines=False, ignore_errors=False,
            errors_to_ignore=(), stdin=''):
    """
    Utility function to execute a command and return the output.

    Arguments:
     * command - command string to execute
     * env - environment, we set locals no to be polish
     * split_lines - On True, the result lines are returned separated
     * ignore_errors - On False, we throw exception if the command failed
     * errors_to_ignore - tuple of errors to ignore
     * stdin - stdin string passed to the command
    """

    if env:
        env.update(os.environ)
    else:
        env = os.environ.copy()

    env['LC_ALL'] = 'en_US.UTF-8'
    env['LANGUAGE'] = 'en_US.UTF-8'

    # Using temporary file is way faster than using subproces.PIPE.
    o = tempfile.TemporaryFile()

    if sys.platform.startswith('win'):
        p = subprocess.Popen(command,
                             stdin=subprocess.PIPE,
                             stdout=o,
                             stderr=subprocess.STDOUT,
                             shell=True,
                             universal_newlines=True,
                             env=env)
    else:
        p = subprocess.Popen(command,
                             stdin=subprocess.PIPE,
                             stdout=o,
                             stderr=subprocess.STDOUT,
                             shell=True,
                             close_fds=True,
                             universal_newlines=True,
                             env=env)
    p.stdin.write(stdin)
    p.stdin.close()

    rc = p.wait()

    o.seek(0)
    data = o.read()
    if split_lines:
        data = data.splitlines()
    if rc and not ignore_errors and rc not in errors_to_ignore:
        raise RuntimeError('Failed to execute command: %s\n%s' % (command, data))

    return data

