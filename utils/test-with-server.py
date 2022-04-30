"""Run client against default player on the actual server."""

import contextlib
import subprocess


@contextlib.contextmanager
def popen(cmd: str):
    """Start a background process and ensure it is running."""
    p = subprocess.Popen(cmd.split(), cwd="server", stdout=subprocess.DEVNULL)
    try:
        p.wait(5)
        raise Exception(cmd + " didn't start")
    except subprocess.TimeoutExpired:
        pass
    try:
        yield p
    finally:
        try:
            p.wait(5)
        except subprocess.TimeoutExpired:
            p.terminate()


if __name__ == "__main__":
    with popen("java -jar server.jar"):
        with popen("java -jar defaultplayer.jar"):
            subprocess.run("./main")
