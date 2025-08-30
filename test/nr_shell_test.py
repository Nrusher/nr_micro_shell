import subprocess
import glob
import os
import sys
import functools
import argparse

def process_testcases(testcases, bin):
    key_log_paths = []
    log_pattern = os.path.join(testcases, "*.log")
    key_log_paths = glob.glob(log_pattern)

    test_print = functools.partial(print, "[test log]")

    test_print(f"find {len(key_log_paths)} logs:")
    for path in key_log_paths:
        test_print(f"  {path}")

    try:
        process = subprocess.Popen(
                    [bin],
                    stdin=subprocess.PIPE,
                    stdout=sys.stdout,
                    stderr=sys.stderr,
                    text=True,
                    bufsize=1,
                    universal_newlines=True
                )
        
        for log_path in key_log_paths:
            test_print(f"testing {log_path}")
            q_num = 0
            with open(log_path, 'r') as f:
                for line in f.readlines():
                    line = line.split(" ")
                    for key_str in line:
                        if key_str == "":
                            continue
                        key = int(key_str, 16)
                        if key == ord('q'):
                            if q_num >= 3:
                                q_num = 0
                                test_print("skip qqqq")
                                continue
                            q_num += 1
                        if process.stdin and not process.stdin.closed:
                            process.stdin.write(chr(key))
                            process.stdin.flush()
                        else:
                            test_print("Warning: stdin is closed, skipping input")

        test_print("test done")
        process.stdin.write('qqqqq')
        process.stdin.flush()
        process.wait(timeout=2)

    except subprocess.TimeoutExpired:
        test_print("Error: Process timeout, force killing...")
        process.kill()
        process.wait()
    except Exception as e:
        test_print(f"Error: Failed to process testcases: {e}")
        if process and process.poll() is None:
            process.kill()
            process.wait()
        exit(1)
    finally:
        if process:
            try:
                if process.poll() is None:
                    process.terminate()
                    try:
                        process.wait(timeout=2)
                    except subprocess.TimeoutExpired:
                        process.kill()
                        process.wait()
            except:
                pass
    return

# Usage example
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Process test cases for nr_shell')
    parser.add_argument('testcases', nargs='?', default='./testcases', 
                        help='Path to test cases directory (default: ./testcases)')
    parser.add_argument('bin', nargs='?', default='../examples/simulator/out/nr_shell_db',
                        help='Path to binary executable (default: ../examples/simulator/out/nr_shell_db)')
    
    args = parser.parse_args()
    process_testcases(args.testcases, args.bin)