#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>

class Dot{
    public:
        int x;
        int y;
        int cls;
        Dot* previous;
        std::string id;
        Dot(int _x,int _y,int _cls,Dot* _dot=NULL)
            :x(_x)
            ,y(_y)
            ,cls(_cls)
            ,previous(_dot)
        {}
        ~Dot(){
            // std::cout<<"descructed!"<<std::endl;
        }
};
class Line{
    public:
    Dot* pre;
    Dot* next;
    Line(Dot* _pre,Dot* _next)
    :pre(_pre)
    ,next(_next)
    {}
};
cv::Mat showimg;
cv::Mat org_img;
std::vector<Dot*> dots;
std::vector<Line*> lines;
std::vector<std::string> checkedlist;

Dot* selected;
int cls=0;
int flag=1;
int x_s=0;
int y_s=0;
int x_e=800;
int y_e=800;
cv::Mat showimgcp;
std::string ReplaceString
(
      std::string String1
    , std::string String2
    , std::string String3
)
{
    std::string::size_type  Pos( String1.find( String2 ) );
 
    while( Pos != std::string::npos )
    {
        String1.replace( Pos, String2.length(), String3 );
        Pos = String1.find( String2, Pos + String3.length() );
    }
 
    return String1;
}
Dot* findpbyid(std::string str){
    for (Dot* dot:dots){
        if (dot->id==str) return dot;
    }
    return NULL;
}
void loadtxt(std::string str){
    std::ifstream ifs(str);
    std::string now="";
    std::vector<std::string> words;
    char c;
    while(ifs.get(c)){
        if (c==','){
            words.push_back(now);
            now="";
        }else if(c=='\n'){
            words.push_back(now);
            now="";
            // for(auto e:words) std::cout<<e<<std::endl;
            if (words.size()==5){
                //add dot
                Dot* dot=new Dot(atoi(words[1].c_str()),atoi(words[2].c_str()),atoi(words[3].c_str()),findpbyid(words[4]));
                dot->id=words[0];
                dots.push_back(dot);
                words.clear();

            }
            else if (words.size()==2){
                //add line
                Line* line=new Line(findpbyid(words[0]),findpbyid(words[1]));
                lines.push_back(line);
                words.clear();
            }
            else{
                printf("txt is broken");
                std::cout<<words.size()<<std::endl;
                exit(1);
            }
            
        }else{
            now+=c;
        }
        

    }
    // for(auto e:dots) std::cout<<e<<" "<<e->id<<" "<<e->x<<" "<<e->y<<" "<<e->cls<<" "<<e->previous<<" "<<std::endl;
    // for(auto e:lines) std::cout<<e->next<<" "<<e->pre<<std::endl;
}
void loadchecked(std::string filename){
    std::ifstream ifs(filename);
    std::string now="";
    char c;
    while(ifs.get(c)){
        if(c=='\n'){
            checkedlist.push_back(now);
            now="";
        }else{
            now+=c;
        }
    }
}
bool in_array(std::vector<std::string> vec, std::string str){
    for(const auto e:vec){
        // std::cout<<e<<str<<(e==str)<<std::endl;
        if (e==str) return 1;
    }
    return 0;
}
void addcheck(std::string str){
    std::ofstream of(".checked",std::ios::app);
    of<<str<<std::endl;
    of.close();
}
void drawlines(){
    // std::cout<<"draw"<<std::endl;
    for(Dot* i:dots){
        cv::Scalar color=i->cls==0?cv::Scalar(0,0,0):cv::Scalar(0,255,0);
        if (i->previous) {
            // std::cout<<i<<std::endl;
            cv::line(showimg,cv::Point(i->previous->x,i->previous->y),cv::Point(i->x,i->y),color,4);
            }
        else{
            circle(showimg,cv::Point(i->x,i->y),5,color,-1);
        }
    }
    if (selected) circle(showimg,cv::Point(selected->x,selected->y),5,cv::Scalar(0,0,255),-1);
    for (Line* i:lines){
        cv::Scalar color=i->pre->cls==0?cv::Scalar(0,0,0):cv::Scalar(0,255,0);
        cv::line(showimg,cv::Point(i->pre->x,i->pre->y),cv::Point(i->next->x,i->next->y),color,4);
        // std::cout<<i->pre->x<<","<<i->pre->y<<","<<i->next->x<<","<<i->next->y<<std::endl;
    }
}
void mouse_callback(int event, int x, int y, int flags, void *userdata){
    if (flag!=2){
        if (event==cv::EVENT_LBUTTONDOWN){

            Dot* dot=new Dot(x_s+x,y_s+y,cls,selected);
            selected=dot;
            dots.push_back(dot);
            // std::cout<<dot<<" "<<dot->x<<" "<<dot->y<<" "<<dot->previous<<std::endl;
            cv::resize(org_img,showimg,cv::Size(800,800));
            drawlines();
            // std::cout<<cv::Point(x_s,y_s)<<cv::Size(std::abs(x_e-x_s),std::abs(y_e-y_s))<<std::endl;
            cv::Rect roi(cv::Point(x_s,y_s),cv::Size(std::abs(x_e-x_s),std::abs(y_e-y_s)));
            showimg=showimg(roi);
            cv::imshow("Display", showimg);
            // std::cout<<selected<<std::endl;
        }
        else if (event==cv::EVENT_RBUTTONDOWN && flags!=cv::EVENT_FLAG_SHIFTKEY){
            // std::cout<<flags<<std::endl;
            Dot* mindot=NULL;
            float d=9999;
            float thresh=100.;
            float min_d=d;
            for(Dot* i:dots){
                d=std::sqrt(std::pow(i->x-(x+x_s),2)+std::pow(i->y-(y+y_s),2));
                    if (thresh>d && min_d>d){
                        mindot=i;
                        min_d=d;
                    }
                // std::cout<<i->x<<" "<<i->y<<" "<<x<<" "<<y<<" "<<d<<" "<<min_d<<std::endl;
                }
            // std::cout<<min_d<<mindot<<std::endl;
            if (min_d!=9999 && mindot!=NULL) {
                // std::cout<<"select"<<std::endl;
                selected=mindot;
            }

            cv::resize(org_img,showimg,cv::Size(800,800));
            drawlines();
            cv::Rect roi(cv::Point(x_s,y_s),cv::Size(std::abs(x_e-x_s),std::abs(y_e-y_s)));
            showimg=showimg(roi);
            cv::imshow("Display", showimg);
        }
        else if (event==cv::EVENT_RBUTTONDOWN && flags==cv::EVENT_FLAG_SHIFTKEY){
            // std::cout<<flags<<std::endl;
            Dot* mindot=NULL;
            float d=9999;
            float thresh=100.;
            float min_d=d;
            for(Dot* i:dots){
                d=std::sqrt(std::pow(i->x-(x+x_s),2)+std::pow(i->y-(y+y_s),2));
                    if (thresh>d && min_d>d){
                        mindot=i;
                        min_d=d;
                    }
                // std::cout<<i->x<<" "<<i->y<<" "<<x<<" "<<y<<" "<<d<<" "<<min_d<<std::endl;
                }
            // std::cout<<min_d<<mindot<<std::endl;
            Line* _line=new Line(selected,mindot);
            lines.push_back(_line);
            for (Line* i:lines){std::cout<<i<<std::endl;}
            if (min_d!=9999 && mindot!=NULL) {
                // std::cout<<"select"<<std::endl;
                selected=mindot;
            }

            cv::resize(org_img,showimg,cv::Size(800,800));
            drawlines();
            cv::Rect roi(cv::Point(x_s,y_s),cv::Size(std::abs(x_e-x_s),std::abs(y_e-y_s)));
            showimg=showimg(roi);
            cv::imshow("Display", showimg);
        }
        else if (event==cv::EVENT_MBUTTONDOWN){
            // std::cout<<"down"<<std::endl;
            if (flag){
                x_s=x+x_s;
                y_s=y+y_s;
                flag=2;
            }
        }
    }
    else if (event==cv::EVENT_MBUTTONUP){
        if (flag==2){
            // std::cout<<"up"<<std::endl;
            cv::Rect roi(cv::Point(x_s,y_s),cv::Size(std::abs(x-x_s),std::abs(y-y_s)));
            // std::cout<<Point(x_s,y_s)<<Size(std::abs(x-x_s),std::abs(y-y_s))<<std::endl;
            x_e=x;
            y_e=y;
            showimg=showimg(roi);
            cv::imshow("Display",showimg);
            flag=0;
        }
    }
    else if (event==cv::EVENT_MOUSEMOVE && flag==2){
        // std::cout<<"move"<<flag<<std::endl;
        showimgcp=showimg.clone();
        cv::rectangle(showimgcp,cv::Point(x_s,y_s),cv::Point(x,y),cv::Scalar(0,0,0));
        cv::imshow("Display",showimgcp);
    }
}


int main(int argc, char** argv )
{
    std::ifstream ifs(argv[1]);
    if (!ifs) {
        std::cerr << "failed to open "<<argv[1] << std::endl;
        std::exit(EXIT_FAILURE);
    }
    loadchecked(".checked");
    bool is_check=(argc==3 && argv[2][0]=='c');

    std::string l;
    int n=0;
    while (std::getline(ifs,l)) {
        n++;
        for (Line* i:lines){ delete i;}
        for (Dot* i:dots){ delete i;}
        lines.clear();
        dots.clear();

        selected=NULL;
        l.erase(std::remove(l.begin(), l.end(), '\n'), l.end());
        std::cout<<n<<","<<l<<std::endl;
        std::ifstream ifs_ch(ReplaceString(l,".jpg",".txt"));
        if (ifs_ch.is_open() && !is_check){
            std::cout<<"skip"<<std::endl;
            continue;
            }
        
        if (in_array(checkedlist,l) && is_check) {
            std::cout<<"skip"<<std::endl;
            continue;
            }
        else if(ifs_ch.is_open()){
            ifs_ch.close();
            std::cout<<"load"<<std::endl;
            loadtxt(ReplaceString(l,".jpg",".txt"));
        }


        org_img = cv::imread( l, 1 );
        cv::resize(org_img,showimg,cv::Size(800,800));

        if ( !org_img.data )
        {
            printf("No image data \n");
            return -1;
        }

        drawlines();
        cv::namedWindow("Display", cv::WINDOW_NORMAL );
        cv::imshow("Display", showimg);
        cv::setMouseCallback("Display",mouse_callback);


        int key;
        while(1){
            key=cv::waitKey(1);
            // printf("%d",flag);
            if(flag!=2){
                if (key=='c'){
                    selected=NULL;
                    cls=0;
                }
                else if (key=='j'){
                    selected=NULL;
                    cls=1;
                }    
                else if (key=='b'){
                    selected=NULL;
                }
                else if(key=='s'){
                    // save img and txt
                    std::ofstream ofs(ReplaceString(l,".jpg",".txt"));
                    for (auto i:dots) ofs<<i<<","<<i->x<<","<<i->y<<","<<i->cls<<","<<i->previous<<std::endl;
                    for (auto i:lines) ofs<<i->pre<<","<<i->next<<std::endl;
                    if(is_check) addcheck(l);
                    break;
                }
                else if (key=='d' && dots.size()!=0){
                    delete dots.back();
                    dots.pop_back();
                    cv::resize(org_img,showimg,cv::Size(800,800));

                    if(dots.size()==0) 
                        selected=NULL;
                    else
                        selected=dots.back();
                    drawlines();
                    cv::Rect roi(cv::Point(x_s,y_s),cv::Size(std::abs(x_e-x_s),std::abs(y_e-y_s)));
                    showimg=showimg(roi);
                    cv::imshow("Display", showimg);

                }
                else if(key=='g' && dots.size()!=0){
                    for(auto e:dots) delete e;
                    dots.clear();
                    for(auto e:lines) delete e;
                    lines.clear();
                    cv::resize(org_img,showimg,cv::Size(800,800));
                    selected=NULL;
                    drawlines();
                    cv::imshow("Display", showimg);


                }
                else if (key=='l' && lines.size()!=0){
                    delete lines.back();
                    lines.pop_back();
                    cv::resize(org_img,showimg,cv::Size(800,800));

                    drawlines();
                    cv::Rect roi(cv::Point(x_s,y_s),cv::Size(std::abs(x_e-x_s),std::abs(y_e-y_s)));
                    showimg=showimg(roi);
                    cv::imshow("Display", showimg);

                }
                else if (key=='z'){
                    x_s=0;
                    y_s=0;
                    x_e=800;
                    y_e=800;
                    cv::resize(org_img,showimg,cv::Size(800,800));
                    drawlines();
                    cv::imshow("Display", showimg);
                    flag=1;
                }
                else if (key=='q'){
                    std::remove(l.c_str());
                    break;
                }
            }
        }
    }
    return 0;
}