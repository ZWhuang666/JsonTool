//ZV Json v0.0.1
/*******************************MIT License************************************
Copyright (c) 2024 Zhiwei Huang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/
//help:
/*
jsonTree is a tree structure to store the json property and values.there are 3 types of node on the tree:
    leaf: a node without any child.
    array: a leaf type node, and the value of the node are inside the "[ ]";
    node: node can be a tree, a leaf ,or a array leaf.
*/
#ifndef _ZV_JSON_h
#define _ZV_JSON_h

#include <iostream>
#include <vector>
//#include "opencv2/core/core.hpp"

#define JS_placeHolder "    "
#define JS_midEndofNode ",\n"
#define JS_finalEndofNode "\n"
#define JS_endofSubTree "}"
#define JS_midEndofSubTree ",\n"
#define JS_finalEndofSubTree "\n"
#define JS_finalEndofTree "}\n"
#define JS_finalStartofTree "{\n"

namespace ZV{

class jsonTree{
    private:
        std::string nodeName="root";
        std::string nodeValue;
        std::vector<jsonTree> node;
        std::string nodeType="node";
        int nodeDepth=0;
        int childQty=0;
        /** @brief when this param set to true, "." in the nodeName will be regard as the connector of mother Node and son Node
         *  @note this param can be changed by function setDotMeaning.
        */
        bool dotAsNodeConnector=true;
        std::string mat2Json(cv::Mat &matrix);        
        jsonTree* findSgl(std::string keyNodeName);
        void renewNodeDepth();
        void getKeysVector(std::string keyName, std::vector<std::string> &keysVector);
    public:
        /** @brief add a leaf Node to the jsonTree*/
        void addLeaf(std::string lNodeName,std::string lNodeValue,std::string lNodeType);
        
        /** @brief create a node from a cv::Mat matrix, and add the node to the jsonTree*/
        void addArr(std::string lNodeName,cv::Mat &matrix);
        
        /** @brief create a node from a cv vector or an Eigen vector,and add the node to the jsonTree
         * @param cols: the dimension of vector.
         * @note: cv_vec type can be any double type cv or Eigen vector ,default is cv::Vec3d or Eigen::Vec3d
        */
        template<typename cv_vec>
        void addArr(std::string lNodeName,cv_vec &vec,int cols=3);

        /** @brief create a node from a std::vector<std::string>, and add the node to the jsonTree*/
        void addArr(std::string lNodeName,std::vector<std::string> &stringVec);

        /** @brief create a node from a std::vector<std::string>, and add the node to the jsonTree*/
        void addArr(std::string lNodeName,std::vector<double> &doubleVec);        

        /** @brief add a node to the jsonTree*/
        void addNode(jsonTree lNode);

        /** @brief set the name of the node*/
        void setName(std::string newName);

        std::string value();

        bool getBoolValue();

        int getIntValue();

        double getDoubleValue();
        
        /** @brief get the matrix from an 2Darray node of the jsonTree*/
        void getArray(std::string keyName,cv::Mat &matrix);
        
        /** @brief get the cv_vec vector from a 1Darray node of the jsonTree*/
        template<typename cv_vec>
        void getArray(std::string keyName,cv_vec &vec,int cols=3);

        /** @brief get the std::vector<std::string> from a 1Darray node of the jsonTree*/
        void getArray(std::string keyName,std::vector<std::string> &stringVec);

        void getArray(std::string keyName,std::vector<double> &doubleVec);
        
        std::string dataType();

        int childs();

        std::string name();

        int depth();

        void json2Mat(std::string jsonStr,cv::Mat &matrix);

        /** @brief find the node named as keyName in the jsonTree*/
        jsonTree* find(std::string keyName);

        /** @brief find the first node with the nodename include the partialKeyNodeName*/
        jsonTree* findFirst(std::string partialKeyNodeName);

        /** @brief convert the jsonTree to a string*/
        std::string to_String();

        /** @brief load a json File to the jsonTree*/
        void load(std::string jsonFilePath,bool removeEnter=false);
        void loadfromString(std::string str,bool removeEnter);
        void loadfromString(std::string str);
        /** @brief save the jsonTree data to a json File*/
        void save(std::string jsonFilePath);
        /** @brief set the value of dotAsNodeConnector*/
        void setDotMeaning(bool dotMeaning) { dotAsNodeConnector=dotMeaning;}

};


inline std::string jsonTree::mat2Json(cv::Mat &matrix){

    int rows=matrix.rows;
    int cols=matrix.cols;
    std::string root="[";
    for (int i = 0; i <rows; i++) {
            std::string rowStr;
            rowStr="[";
                        
            for (int j = 0; j < cols; j++) {

                rowStr=rowStr+std::to_string(matrix.at<double>(i,j));
                if (j<cols-1) rowStr=rowStr+",";
                else rowStr=rowStr+"]";
            }
            root=root+rowStr;
            if(i<rows-1) root=root+",";
            else root=root+"]";
                        
    }
    return root;


}
inline void jsonTree::loadfromString(std::string str,bool removeEnter){
    std::string str2;
    for(unsigned int i=0;i<str.size();i++){
        if(str[i]=='\n'){
            if(str[i-1]=='}' || str[i+1]=='}'){str2.push_back(str[i]);}
            else if(str[i-1]==',' && str[i-2]==']' && str[i-3]==']'){str2.push_back(str[i]);}
            else if(str[i-1]==',' && str[i-2]==']' && str[i+1]=='\"'){str2.push_back(str[i]);}
            else if(str[i+1]=='\"'){str2.push_back(str[i]);}
        }
        else{
            str2.push_back(str[i]);
        }
    }
    if(removeEnter) loadfromString(str2);
    else loadfromString(str);

}

inline void jsonTree::loadfromString(std::string str){
    //read nodes
    const char nodeStart='{';
    const char arrStart='[';
    const char nodeEnd='}';
    size_t p=str.find(":");

    while(p!=std::string::npos){
                char t=str[str.find_first_not_of(" ",p+1)];
                std::string nodeName;
                size_t nodeNameEnd=str.find_last_of("\"",p-1);
                size_t nodeNameStart=str.find_last_of("\"",nodeNameEnd-1);
                nodeName=str.substr(nodeNameStart+1,nodeNameEnd-1-(nodeNameStart));
                //std::cout<<"nodeName="<<nodeName<<"  ,endofNodeName\n";
                std::string leafValue;
                size_t c,n2,n3;
                int counts1=0,counts2=0;
                jsonTree newNode;
                switch (t){
                    case nodeStart:
                        //get the node string between{}
                        counts1=1;
                        counts2=0;
                        //jump the blanks if exist.
                        p=str.find_first_not_of(" ",p+1)-1;
                        n2=p;
                        while(counts1>counts2){
                            n3=n2+1;
                            n2=str.find_first_of(nodeEnd,n3);
                            counts2++; 
                            for(int m=n3+1;m<n2;m++){
                                if(str[m]==nodeStart) counts1++;
                            }
                        }
                        //      
                        newNode.loadfromString(str.substr(p+1,n2-p));
                        //add the node.
                        newNode.setName(nodeName);
                        this->addNode(newNode);
                        //jump the point to the end of the node
                        p=n2;
                        break;
                    case arrStart:
                        c=str.find("\n",p+1);
                        if(str[c-1]==',') c=c-1;
                        p=str.find_first_not_of(" ",p+1)-1;
                        leafValue=str.substr(p+1,c-1-p);

                        //std::cout<<"arrname="<<nodeName<<",arrvalue="<<leafValue<<"\n";
                        this->addLeaf(nodeName,leafValue,"array");
                        //jump to the end of the arr
                        p=c;                        
                        break;

                    default:
                        c=str.find("\n",p+1);
                        if(str[c-1]==',') c=c-1;
                        //get rid of blanks
                        p=str.find_first_not_of(" ",p+1)-1;
                        leafValue=str.substr(p+1,c-1-p);

                        //std::cout<<"leafname="<<nodeName<<",leafvalue="<<leafValue<<"\n";
                        this->addLeaf(nodeName,leafValue,"Notstring");

                        //jump to the end of this leaf;
                        p=c;                    
                }
                
        p=str.find(":",p+1);
    };            
}


inline void jsonTree::addLeaf(std::string lNodeName,std::string lNodeValue,std::string lNodeType){
            jsonTree newNode;
            newNode.nodeName=lNodeName;
            newNode.nodeValue=lNodeValue;

            newNode.nodeType=lNodeType;
            if(lNodeType=="bool"||lNodeType=="BOOL"){
                if(lNodeValue=="1") newNode.nodeValue="true";
                if(lNodeValue=="0") newNode.nodeValue="false";
            }
            newNode.nodeDepth=nodeDepth+1;
            node.push_back(newNode);
            childQty++;

}
inline void jsonTree::addArr(std::string lNodeName,cv::Mat &matrix){
            jsonTree newNode;
            newNode.nodeName=lNodeName;
            newNode.nodeValue=this->mat2Json(matrix);
            if (newNode.nodeValue=="[]") return;
            newNode.nodeType="array2D";
            newNode.nodeDepth=nodeDepth+1;
            node.push_back(newNode);
            childQty++;

}

template<typename cv_vec>
inline void jsonTree::addArr(std::string lNodeName,cv_vec &vec,int cols){
            jsonTree newNode;
            newNode.nodeName=lNodeName;
            //int rows=3;
            std::string root="[";
            for (int i = 0; i <cols; i++) {
                root+=std::to_string(vec[i]);
                if(i<cols-1) root+=",";
                else root+="]";            
             }
    
            newNode.nodeValue=root;
            newNode.nodeType="array1D";
            newNode.nodeDepth=nodeDepth+1;
            node.push_back(newNode);
            childQty++;
}
inline void jsonTree::addArr(std::string lNodeName,std::vector<double> &vec){
            jsonTree newNode;
            newNode.nodeName=lNodeName;
            int rows=vec.size();
            
            std::string root="[";
            if(rows==0) {root="[\"None\"]";return;}
            for (int i = 0; i <rows; i++) {
                root+=std::to_string(vec[i]);
                if(i<rows-1) root+=",";
                else root+="]";            
             }
    
            newNode.nodeValue=root;
            newNode.nodeType="array1D";
            newNode.nodeDepth=nodeDepth+1;
            node.push_back(newNode);
            childQty++;
}
inline void jsonTree::addArr(std::string lNodeName,std::vector<std::string> &stringVec){
            jsonTree newNode;
            newNode.nodeName=lNodeName;
            int rows=stringVec.size();
            std::string root="[";
            for (int i = 0; i <rows; i++) {
                root+="\""+stringVec[i]+"\"";
                if(i<rows-1) root+=",";
                else root+="]";            
             }
    
            newNode.nodeValue=root;
            newNode.nodeType="array1D";
            newNode.nodeDepth=nodeDepth+1;
            node.push_back(newNode);
            childQty++;
}

inline void jsonTree::renewNodeDepth(){
            for(int i=0;i<childQty;i++){
                if(node[i].childQty==0){
                    node[i].nodeDepth++;
                }
                else{
                    node[i].nodeDepth++;
                    node[i].renewNodeDepth();
                }
            }
}
inline void jsonTree::addNode(jsonTree lNode){
            lNode.nodeDepth=nodeDepth+1;
            lNode.renewNodeDepth();
            node.push_back(lNode);
            childQty++;
}

inline void jsonTree::setName(std::string newName){
            this->nodeName=newName;
}
inline jsonTree* jsonTree::findSgl(std::string keyNodeName){
            jsonTree* foundNode;
            jsonTree* p=this;
            int iPos=0;
            std::vector<std::pair<jsonTree*,int>> p2; 
            if (this->nodeName==keyNodeName){
                foundNode=this;
                return foundNode;
            }
                            
            while(p->childQty>0){
                for(int j=iPos;j<p->childQty;j++){
                    if(p->node[j].nodeName==keyNodeName){
                        foundNode=&(p->node[j]);
                        return foundNode;
                    }
                }
                    
                bool allLeaf=true;
                for(int j=iPos;j<p->childQty;j++){
                    if(p->node[j].childQty>0){                                
                        if(j<p->childQty-1){                           
                            p2.push_back(std::make_pair(p,j+1));                           
                        }
                        p=&(p->node[j]);
                        iPos=0;
                        allLeaf=false;
                        break;
                    }
                }
                if(allLeaf){
                    if(p2.size()>0){
                        p=p2[p2.size()-1].first;
                        iPos=p2[p2.size()-1].second;
                        p2.pop_back();
                    }
                    else{
                        foundNode=nullptr;
                        break;
                    }
                }
                    
            }
            return foundNode;
}

inline jsonTree* jsonTree::findFirst(std::string partialKeyNodeName){
            jsonTree* foundNode;
            jsonTree* p=this;
            int iPos=0;
            std::vector<std::pair<jsonTree*,int>> p2; 
            if (this->nodeName.find(partialKeyNodeName)!= std::string::npos){
                foundNode=this;
                return foundNode;
            }
                            
            while(p->childQty>0){
                for(int j=iPos;j<p->childQty;j++){
                    if(p->node[j].nodeName.find(partialKeyNodeName)!= std::string::npos){
                        foundNode=&(p->node[j]);
                        return foundNode;
                    }
                }
                    
                bool allLeaf=true;
                for(int j=iPos;j<p->childQty;j++){
                    if(p->node[j].childQty>0){                                
                        if(j<p->childQty-1){                           
                            p2.push_back(std::make_pair(p,j+1));                           
                        }
                        p=&(p->node[j]);
                        iPos=0;
                        allLeaf=false;
                        break;
                    }
                }
                if(allLeaf){
                    if(p2.size()>0){
                        p=p2[p2.size()-1].first;
                        iPos=p2[p2.size()-1].second;
                        p2.pop_back();
                    }
                    else{
                        foundNode=nullptr;
                        break;
                    }
                }
                    
            }
            return foundNode;
}                  

inline std::string jsonTree::value(){
            std::string s=nodeValue;
            size_t p1=s.find_first_not_of(" ",0);
            if (s[p1]=='\"') s=s.substr(p1+1,s.length()-p1-1);
            size_t p2=s.find_last_not_of(" ");
            if(s[p2]=='\"') s=s.substr(0,p2);            
            return s;
}
inline bool jsonTree::getBoolValue(){
            if(nodeValue=="true"){
                return true;
            }
            else return false;
}
inline int jsonTree::getIntValue(){
            return std::stoi(nodeValue);
}
inline double jsonTree::getDoubleValue(){
            return std::stod(nodeValue);
}
inline std::string jsonTree::dataType(){
            return nodeType;
}
inline int jsonTree::childs(){
            return childQty;
}
inline std::string jsonTree::name(){
            return nodeName;
}
inline int jsonTree::depth(){
            return nodeDepth;
}
inline void jsonTree::getArray(std::string keyName,cv::Mat &matrix){
            jsonTree* pTree;
            
            pTree=find(keyName);
            if(pTree==nullptr){
                std::cout<<keyName<<"  not found!"<<"\n";
                return;
            }
            //else std::cout<<"pTree.nodeName="<<pTree->nodeName<<"\n";
            if(pTree->childQty>0) {
                std::cout<<"error,the keyName node is not an array\n";
                return;
            }

            std::string value=pTree->nodeValue;
            
            json2Mat(value,matrix);


}
template<typename cv_vec>
inline void jsonTree::getArray(std::string keyName,cv_vec &vec,int cols){
            jsonTree* pTree;
            
            pTree=find(keyName);
            if(pTree==nullptr){
                std::cout<<keyName<<"  not found!"<<"\n";
                return;
            }
            
            if(pTree->childQty>0) {
                std::cout<<"error,the keyName node is not an array\n";
                return;
            }

            std::string value=pTree->nodeValue;
            
            size_t p1=value.find_first_of("[");
            size_t p2=value.find_last_of("]");
            std::string data=value.substr(p1+1,p2-p1);
            
                size_t sPos=0;
                int j=0;
                for(size_t iPos=data.find(",");iPos!=std::string::npos;iPos=data.find(",",iPos+1)){
                    std::string leaf=data.substr(sPos,iPos-sPos);
                    sPos=iPos+1;
                    if(j<cols-1){
                        vec[j]=std::stod(leaf);
                    }
                    else{
                        std::cout<<"error,data more than 3.";
                        return;
                    }

                    j++;
                }
                
                vec[j]=std::stod(data.substr(sPos,data.length()-sPos+1));

}
inline void jsonTree::getArray(std::string keyName,std::vector<std::string> &stringVec){
            jsonTree* pTree=find(keyName);
            if(pTree==nullptr){
                std::cout<<keyName<<"  not found!"<<"\n";
                return;
            }            
            if(pTree->childQty>0) {
                std::cout<<"error,the keyName node is not an array\n";
                return;
            }

            std::string value=pTree->nodeValue;            
            size_t p1=value.find_first_of("[");
            size_t p2=value.find_last_of("]");
            std::string data=value.substr(p1+1,p2-p1-1);
            
                size_t sPos=0;
                int j=0;
                for(size_t iPos=data.find(",");iPos!=std::string::npos;iPos=data.find(",",iPos+1)){
                    std::string leaf=data.substr(sPos,iPos-sPos);
                    size_t p3=leaf.find_first_not_of(" ",0);
                    if (leaf[p3]=='\"') leaf=leaf.substr(p3+1,leaf.length()-p3-1);
                    size_t p4=leaf.find_last_not_of(" ");
                    if(leaf[p4]=='\"') leaf=leaf.substr(0,p4);
                    sPos=iPos+1;
                    stringVec.push_back(leaf);
                    j++;
                }
                
                std::string leafEnd=data.substr(sPos,data.length()-sPos+1);
                p1=leafEnd.find_first_not_of(" ",0);
                if (leafEnd[p1]=='\"') leafEnd=leafEnd.substr(p1+1,leafEnd.length()-p1-1);
                p2=leafEnd.find_last_not_of(" ");
                if(leafEnd[p2]=='\"') leafEnd=leafEnd.substr(0,p2);                
                stringVec.push_back(leafEnd);
}
inline void jsonTree::getArray(std::string keyName,std::vector<double> &doubleVec){
            jsonTree* pTree=find(keyName);
            if(pTree==nullptr){
                std::cout<<keyName<<"  not found!"<<"\n";
                return;
            }            
            if(pTree->childQty>0) {
                std::cout<<"error,the keyName node is not an array\n";
                return;
            }

            std::string value=pTree->nodeValue;            
            size_t p1=value.find_first_of("[");
            size_t p2=value.find_last_of("]");
            if (p2==p1+1) return;//empty array
            std::string data=value.substr(p1+1,p2-p1);
            
                size_t sPos=0;
                int j=0;
                for(size_t iPos=data.find(",");iPos!=std::string::npos;iPos=data.find(",",iPos+1)){
                    double leaf=std::stod(data.substr(sPos,iPos-sPos));
                    sPos=iPos+1;
                    doubleVec.push_back(leaf);
                    j++;
                }
                
                doubleVec.push_back(std::stod(data.substr(sPos,data.length()-sPos+1)));
}
inline void jsonTree::json2Mat(std::string jsonStr,cv::Mat &matrix)
        {
   
            std::vector<std::string> subarr;
            size_t endPos=jsonStr.find_last_of("]");
            for(size_t iPos=jsonStr.find("]");iPos<endPos;iPos=jsonStr.find("]",iPos+1)){
                  size_t jPos=jsonStr.find_last_of("[",iPos-1);
                  subarr.push_back(jsonStr.substr(jPos+1,iPos-jPos-1));  
                  
            }
            
            for(unsigned int i=0;i<subarr.size();i++)
            {
                size_t sPos=0;
                int j=0;
                for(size_t iPos=subarr[i].find(",");iPos!=std::string::npos;iPos=subarr[i].find(",",iPos+1)){
                    std::string leaf=subarr[i].substr(sPos,iPos-sPos);
                    sPos=iPos+1;
                    
                    matrix.at<double>(i,j)=std::stod(leaf);
                    j++;
                }
                
                matrix.at<double>(i,j)=std::stod(subarr[i].substr(sPos,subarr[i].length()-sPos+1));
            }

}        
inline jsonTree* jsonTree::find(std::string keyName){
            std::vector<std::string> keys;
            getKeysVector(keyName,keys);
            jsonTree* pTree;
            pTree=this;
            unsigned int i=0;
            while(i<keys.size()){
                if(pTree!=nullptr){
                    //std::cout<<"start to find "<<keys[i]<<"\n";
                    pTree=pTree->findSgl(keys[i]);
                    if (pTree!=nullptr) ;//std::cout<<"found "<<pTree->nodeName<<"\n";
                    else std::cout<<keys[i]<<" not found\n";
                }
                else break;
                i++;
            }
            if(pTree!=nullptr && pTree->nodeName==keys[keys.size()-1]){
                return pTree;
            }
            else {
                std::cout<<keyName<<" not found.\n";
                return nullptr;
            }
}
inline void jsonTree::getKeysVector(std::string keyName, std::vector<std::string> &keysVector){
            std::string a=keyName;
            size_t p=a.find(".");
            if(p==std::string::npos || (!dotAsNodeConnector)){
                keysVector.push_back(a);
                return;
            }
            size_t pStart=0;
            
            while(p!=std::string::npos){
                keysVector.push_back(a.substr(pStart,p-pStart));
                pStart=p+1;
                p=a.find(".",pStart);

            }
            keysVector.push_back(a.substr(pStart,a.length()-pStart+1));            
}
inline std::string jsonTree::to_String(){
            std::string oStr;    
            if(this->nodeDepth==0) oStr=JS_finalStartofTree;

            //totalLevel=1;
            //std::cout<<"tlevel="<<this->nodeDepth<<",this.name="<<this->nodeName<<",this.childqty="<<this->node.size()<<"\n";
            if(this->childQty==0){
                    //print placeholder
                    for(int x=0;x<=this->nodeDepth;x++){
                        oStr+=JS_placeHolder;
                    }
                    //print nodeName
                    oStr+="\""+this->nodeName+"\""+":";
                    //print nodeValue
                    if(this->nodeType=="std::string"){
                            oStr+="\""+this->nodeValue+"\"";
                    }         
                    else oStr+=this->nodeValue;            
                    //print nodeEnd
                    
                    oStr+=JS_finalEndofNode;                
            }
            for(int j=0;j<this->childQty;j++){
                //std::cout<<"nodename="<<node[j].nodeName<<",level="<<node[j].nodeDepth<<",this.childqty="<<node[j].node.size()<<"\n";
                if(this->node[j].childQty==0){
                    //print placeholder
                    for(int x=0;x<=this->nodeDepth;x++){
                        oStr+=JS_placeHolder;
                    }
                    //print nodeName
                    oStr+="\""+this->node[j].nodeName+"\""+":";
                    //print nodeValue
                    if(this->node[j].nodeType=="std::string"){
                            oStr+="\""+this->node[j].nodeValue+"\"";
                    }         
                    else oStr+=this->node[j].nodeValue;            
                    //print nodeEnd
                    if(j<this->childQty-1) oStr+=JS_midEndofNode;
                    else oStr+=JS_finalEndofNode;

                }
                else{
                    //print tree placeHolder
                    for(int x=0;x<=this->nodeDepth;x++){
                        oStr+=JS_placeHolder;
                    }
                    //print tree name
                    oStr+="\""+this->node[j].nodeName+"\""+":";
                    //print tree start
                    oStr+=JS_finalStartofTree;
                    //print sub tree
                    oStr+=this->node[j].to_String();
                    for(int x=0;x<=this->nodeDepth;x++){
                        oStr+=JS_placeHolder;
                    }        
                    //print sub tree end }            
                    oStr+=JS_endofSubTree;
                    //print sub tree end,
                    if(j==this->childQty-1) oStr+=JS_finalEndofSubTree;
                    else oStr+=JS_midEndofSubTree;
                }
            }

            if(this->nodeDepth==0) oStr+=JS_finalEndofTree;
            return oStr;
            
}  

inline void jsonTree::save(std::string jsonFilePath){
           
            std::ofstream out(jsonFilePath.c_str());  
            out<<this->to_String().c_str();
            out.close();    
}   

inline void jsonTree::load(std::string jsonFilePath,bool removeEnter){
            //read the json file to a std string
            std::ifstream file(jsonFilePath);
            if(!file.good()){
                std::cout<<jsonFilePath<<"  not found!";
                return;
            }
            std::istreambuf_iterator<char> beg(file), end; 
            std::string str(beg, end);
            file.close();
            //load the string to the jsonTree
            if(removeEnter) loadfromString(str,removeEnter);
            else loadfromString(str);
            //std::cout<<"tree="<<this->to_String()<<"\n";
}    

} //end of namespace ZV
#endif 