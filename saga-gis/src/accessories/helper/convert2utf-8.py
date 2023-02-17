import os, glob, codecs

#________________________________________
def has_BOM(File):
    f = open(File, 'rb'); b = f.read(len(codecs.BOM_UTF8)); f.close()
    return b == codecs.BOM_UTF8

#________________________________________
def del_BOM(File):
    if has_BOM(File):
        f = open(File, 'rb'); f.read(len(codecs.BOM_UTF8)); c = f.read(); f.close()
        f = open(File, 'wb'); f.write(c); f.close()
        return True
    return False

#________________________________________
def add_BOM(File):
    if not has_BOM(File):
        f = open(File, 'rb'); c = f.read(); f.close()
        f = open(File, 'wb'); f.write(codecs.BOM_UTF8); f.write(c); f.close()
        return True
    return False

#________________________________________
def is_ANSI(File):
    f = codecs.open(File, 'r', encoding = 'utf8')
    try:
        f.read(); f.close()
        return False #; print('is already utf-8 conform: ' + os.path.split(File)[1])
    except:
        f.close()
        return True  #; print('is not utf-8 conform: ' + os.path.split(File)[1])

#________________________________________
def Convert_To_UTF8(File):
    if is_ANSI(File):
        with open(File, 'r') as f:
            c = f.read(); f.close()
        with codecs.open(File, 'w', encoding = 'utf8') as f:
            f.write(c); f.close()
        return True
    return False

#________________________________________
def Convert_Files(Root, Extensions = ['cpp', 'c', 'h', 'hpp']):
    n = 0
    for Extension in Extensions:
        Files = glob.glob('{:s}/**/*.{:s}'.format(Root, Extension), recursive=True)
        for File in Files:
            # if has_BOM(File):
            #     print(File.replace(Root, '.'))
            # if add_BOM(File):
            #     print(File.replace(Root, '.'))
            #     n += 1
            # if del_BOM(File):
            #     print(File.replace(Root, '.'))
            #     n += 1

            # if is_ANSI(File):
            #     print(File.replace(Root, '.'))
            if Convert_To_UTF8(File):
                print(File.replace(Root, '.'))
                n += 1
    print('{:d} replacement(s)'.format(n))
    return True

#________________________________________
Convert_Files('F:/develop/saga/saga-code/master/saga-gis/src')
