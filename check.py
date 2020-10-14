import csv
import matplotlib.pyplot as plt
import os
from PIL import Image
class Dot:
    def __init__(self,d):
        d=d.split(',')
        self.dot_id=d[0]
        self.x=int(d[1])
        self.y=int(d[2])
        self.cls=int(d[3])
        self.next=d[4].strip()
class Line:
    def __init__(self,d):
        d=d.split(',')
        self.pre=d[0]
        self.next=d[1].strip()
def getcoor(data,dot_id):
    for d in data:
        if type(d)==Dot and d.dot_id==dot_id:
            return d.x,d.y

def getcls(data,dot_id):
    for d in data:
        if type(d)==Dot and d.dot_id==dot_id:
            return d.cls
def show(path):
    plt.imshow(Image.open(path).resize((800,800)))
    txtp=path.replace('.jpg','.txt')
    if os.path.exists(txtp):
        with open(txtp) as ft:
            data=[Dot(i) if len(i.split(','))==5 else Line(i) for i in ft.readlines()]
        for d in data:
            if type(d)==Dot and d.next!='0':
               pre=getcoor(data,d.next)
               plt.plot([d.x,pre[0]],[d.y,pre[1]],c='k' if d.cls==0 else 'g')
            elif type(d)==Line:
                pre=getcoor(data,d.pre)
                nx=getcoor(data,d.next)
                print(getcls(data,d.pre),pre,nx)
                plt.plot([pre[0],nx[0]],[pre[1],nx[1]],c='k' if getcls(data,d.pre)==0 else 'g')
        plt.show()

if __name__=='__main__':
    with open('list') as f:
        [show(i.strip()) for i in f.readlines()]