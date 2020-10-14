import numpy as np
import cv2 
import torch
import glob
from PIL import Image
import torchvision.transforms as T
class LinerCrackDataset(torch.utils.data.Dataset):
    def __init__(self,root):
        self.txt=glob.glob(f'{root}/*.txt')
        self.transform=T.Compose([T.ToTensor()])
    def __len__(self):
        return len(self.txt)
    def __getitem__(self,idx):
        im=Image.open(self.txt[idx].replace('txt','jpg'))
        mask=loadtxt(self.txt[idx])
        return self.transform(im),mask
def loadtxt(path):
    thickness=21
    def getdata(ind,sec='point'):
        for d in data:
            print(d[0],ind)
            if d[0]==ind:
                if sec=='point':return int(d[1]),int(d[2])
                elif sec=='cls':return int(d[3])
        assert False,f"{idx} is not found."
    mask=np.zeros((800,800))
    with open(path) as f:
        data=[d.strip().split(',') for d in f.readlines()]
    print(data)
    for d in data:
        if d[-1]=='0': continue
        if len(d)==5:
           cv2.line(mask,(int(d[1]),int(d[2])),getdata(d[-1]),color=int(d[3])+1,thickness=thickness)
        elif len(d)==2:
            cv2.line(mask,getdata(d[0]),getdata(d[1]),thickness=thickness,color=getdata(d[0],'cls')+1)
    return mask

if __name__=='__main__':
    mask=loadtxt('7.txt')
    print((mask).max())
    import matplotlib.pyplot as plt
    plt.imshow(mask)
    plt.show()