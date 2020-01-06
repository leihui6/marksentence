import os

path = '.\\'

files = []

def load_all_file():
    # r=root, d=directories, f = files
    for r, d, f in os.walk(path):
        for file in f:
            files.append(os.path.join(r, file))

def get_audio_type(s):
    ret_list = []
    tpo_pos = s.find("tpo",s.rfind("\\"))
    underline_pos = s.find("_")
    # get the number of TPO
    # print(s[tpo_pos+3:underline_pos])
    ret_list.append(s[tpo_pos+3:underline_pos].zfill(2))
    
    # get the type of TPO
    order_tpo = s[s.find("ing",tpo_pos)+3:s.find("_",underline_pos+1)]
    order_list = ['C1','L1','L2','C2','L3','L4']
    ret_list.append(order_list[int(order_tpo)-1])
    return ret_list

if __name__ == '__main__':
    load_all_file()
    # Delete all file which name not contain "passage" and contain "writing".
    #
    # for f in files:
    #    print(f)
    #   if "passage" not in f:
    #        os.remove(f)
    #   elif "writing" in f:
    #        os.remove(f)
    
    # Change the name into TPO[N][TYPE]
    for f in files:
        if '.py' in f:
            continue
        prefix = f[0:f.rfind("\\")]
        TPO_INFO = get_audio_type(f)
        # print (TPO_INFO[0],TPO_INFO[1])
        print(prefix + "\\"+'TPO'+TPO_INFO[0]+TPO_INFO[1]+".mp3")
        os.rename(f,prefix + "\\"+'TPO'+TPO_INFO[0]+TPO_INFO[1]+".mp3")
        
