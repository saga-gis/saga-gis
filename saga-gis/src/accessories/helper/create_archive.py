#________________________________________
def install(package):
    import subprocess, sys; subprocess.check_call([sys.executable, '-m', 'pip', 'install', package])

#________________________________________
def Create_Tarball(dir):
    import os, tarfile
    print('creating tarball \'{:s}\'...'.format(os.path.split(dir)[1]), end='', flush=True)
    tmp = tarfile.open('tmp.tar', 'w')
    tmp.add(dir, arcname=os.path.basename(dir))
    tmp.close()
    tmp = tarfile.open('tmp.tar', 'r')
    with tarfile.open(dir + '.tar.gz', 'w:gz') as tar:
        for entry in tmp.getmembers():
            if entry.isfile():
                entry.mode = 0o644
            else:
                entry.mode = 0o755
            tar.addfile(entry, tmp.extractfile(entry))
        tar.close()
    tmp.close()
    os.remove('tmp.tar')
    print('ready')

#________________________________________
def Create_ZipFile(dir):
    import os; from shutil import make_archive
    print('creating zipfile \'{:s}\'...'.format(os.path.split(dir)[1]), end='', flush=True)
    make_archive(dir, 'zip', None, dir)
    print('ready')

#________________________________________
if __name__ == '__main__':
    import sys, os
    if len(sys.argv) == 2 and os.path.exists(sys.argv[1]):
        Create_ZipFile(sys.argv[1])

    if len(sys.argv) >= 3 and os.path.exists(sys.argv[2]):
        if sys.argv[1] == 'tar':
            Create_Tarball(sys.argv[2])
        else:
            Create_ZipFile(sys.argv[2])
