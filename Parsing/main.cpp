/*
Travell Wright <tdw53@zips.uakron.edu>
Date: 3/7/2016
*/

#include <string>
#include <vector>
#include <iostream>
#include <fstream>


struct Value
{
    virtual int weight()
    {
        return 1;
    }

    virtual Value* parse(std::string, int&, int&) =0;

    virtual ~Value()=default;
};


struct Bool: public Value
{
    std:: string boolean;
    Value* parse(std::string data, int &k, int &test)
    {
        boolean = "";
        while(data[k]=','&& data[k]!='}' && data[k]!=']')
        {
            boolean += data[k];
            k++;
        }
        test++;
        return this;
    }
};

struct String: public Value
{
  std::string str;
  Value* parse(std::string data, int &k, int &test)
  {
    str="";
    k++;
    while(data[k]!='"')
    {
        if(data[k]=='\\')
            k++;
        str+=data[k];
        k++;
    }
    test++;
    return this;
  }
};

struct Number: public Value
{
  std::string number;

  Value* parse(std::string data, int &k, int &test)
  {
    number = "";
    while(data[k]=='-' || isdigit(data[k]) || data[k]=='e' || data[k]=='.')
    {
        number+=data[k];
        k++;
    }
    test++;
    return this;
  }
};


struct Null: public Value
{
  std::string null;
  Value* parse(std::string data, int &k, int &test)
  {
     null="";
     while(data[k]!=','&&data[k]!='}'&&data[k]!=']')
     {
         null+=data[k];
         k++;
     }
     test++;
     return this;
    }
};


struct Array: public Value, std::vector<Value*>
{
    std::vector<Value*> jsonArray;
    Value* parse(std::string, int&,int&);

    int weight();
};

struct Object: public Value
{
    std::vector<std::pair<std::string, Value*>> j_Object;
    Value* parse(std::string data, int& k, int &test)
    {

        Object contObject;
        Array contArray;
        Number contNumber;
        Bool contBool;
        Null contNull;
        String contString;

        std::string valueName="";
        std::pair<std::string, Value*> orderedPair;
        int counter=0;

        while(data[k]!='}')
        {
            while (valueName=="")
            {
                k++;
                if(data[k]=='"')
                {
                    k++;
                    while(data[k]!='"')
                        {
                            if(data[k]=='\\')
                            {
                                k++;
                            }
                            valueName+=data[k];
                            k++;
                        }
                    k++;
                }
            }
            if(data[k]=='"')
            {
                orderedPair= std::make_pair(valueName,contString.parse(data,k,test));
                j_Object.push_back(orderedPair);
                valueName = "";
                counter++;
            }

            else if(data[k]=='-' || isdigit(data[k]))
            {
                orderedPair=std::make_pair(valueName,contNumber.parse(data,k,test));
                j_Object.push_back(orderedPair);
                valueName = "";
                counter++;
            }
            else if(data[k]=='t'||data[k]=='f')
            {
                orderedPair = std::make_pair(valueName,contBool.parse(data,k,test));
                j_Object.push_back(orderedPair);
                valueName = "";
                counter++;
            }
            else if(data[k]=='n')
            {
                orderedPair = std::make_pair(valueName,contNull.parse(data,k,test));
                j_Object.push_back(orderedPair);
                valueName = "";
                counter++;
            }
            else if(data[k]=='[')
            {
                orderedPair = std::make_pair(valueName,contArray.parse(data,k,test));
                j_Object.push_back(orderedPair);
                valueName = "";
                counter++;
            }
            else if(data[k]=='{')
            {
                orderedPair = std::make_pair(valueName,contObject.parse(data, k,test));
                j_Object.push_back(orderedPair);
                valueName = "";
                counter++;
            }
            if(data[k]=='}')
            {
                test++;
                return this;
            }
            k++;
        }
        test++;
        test+=counter;
        return this;
}

    int weight()
    {
        std::cout<<"Getting Weight of Object.."<<std::endl;
        int total = 1;

        for(int i=0; i<j_Object.size();i++)
        {
            total+=j_Object[i].second->weight();
        }
        return total;
    }
};

Value* Array::parse(std::string data, int &k,int& test)
{
        Object contObject;
        Array contArray;
        Number contNumber;
        Bool contBool;
        Null contNull;
        String contString;

        std::string valueName;
        int counter = 0;

        while(data[k]!=']')
        {
            k++;
            if(data[k]=='"')
            {
                jsonArray.push_back(contString.parse(data,k,test));
                counter++;
            }
            else if(data[k]=='-'||isdigit(data[k]))
            {
                jsonArray.push_back(contNumber.parse(data,k,test));
                counter++;
            }
            else if(data[k]=='t'||data[k]=='f')
            {
                jsonArray.push_back(contBool.parse(data,k,test));
                counter++;
            }
            else if(data[k]=='n')
            {
                jsonArray.push_back(contNull.parse(data,k,test));
                counter++;
            }
            else if(data[k]=='[')
            {
                jsonArray.push_back(contArray.parse(data,k,test));
                counter++;
            }
            else if(data[k]=='{')
            {
                jsonArray.push_back(contObject.parse(data, k,test));
                counter++;
            }
        }
        test++;
        test+=counter;
        return this;
}

int Array::weight()
{
    int total=1;
    int vSize=this->size();

    for(int i=0; i<vSize; i++)
    {
        total += this[i].weight();
    }
    return total;
}


int main(int argc, char*argv[])
{
    Bool jbool;
    String jstring;
    Array jarray;
    Null jnull;
    Object jobject;
    Number jnumber;

    std::vector<Value*> jstructure;

    int weight = 0;

    int total=0;

    std::string fileName = argv[1];
    std::ifstream json;

    json.open(fileName.c_str(),std::ios::binary);

    std::streampos begin, end;

    begin=json.tellg();
    json.seekg(0,std::ios::end);
    end=json.tellg();

    std::streampos size = end-begin;
    json.seekg(0,std::ios::beg);

    char * fileData = new char[size];
    json.read(fileData, size);
    fileData[size]='\0';
    json.close();

    std::string data(fileData);

    int jsonSize =data.size();

    int k=0;
    std::cout<<"Begin Processing of JSON File"<<std::endl;
    do
    {
        switch(data[k])
        {
            case '{':
                jstructure.push_back(jobject.parse(data, k, total));
                break;

            case '[':
                jstructure.push_back(jarray.parse(data,k, total));
                break;

            case '"':
                jstructure.push_back(jstring.parse(data,k, total));
                break;

            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':jstructure.push_back(jnumber.parse(data,k,total));
                break;

            case 't':
            case 'f':jstructure.push_back(jbool.parse(data,k,total));
                break;

            case 'n':jstructure.push_back(jnull.parse(data,k,total));
                break;

            default:
                break;
        }
        k++;
    }
    while(k<jsonSize);

    std::cout<<"Processing Complete"<<std::endl;


    int vSize=jstructure.size();

    std::cout<<"File Weight: " << fileName <<" is: "<<total<<std::endl;

    return 0;
}
