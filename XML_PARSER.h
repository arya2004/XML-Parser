//
// Created by ziegler on 8/21/23.
//


#ifndef XML_PARSER_SPEEDRUN_XML_PARSER_H
#define XML_PARSER_SPEEDRUN_XML_PARSER_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef TRUE
    #define TRUE 1
#endif
#ifndef FALSE
    #define  FALSE 0
#endif

typedef struct IAttribute{
    char * key;
    char * value;
}Attribute;

void freeAttribute(Attribute* _attribute);

typedef struct IAttributeVector{
    int heap_size;
    int size;
    Attribute * data;
}AttributeVector;

void InitAttributes(AttributeVector* _attrs);
void AddAttribute(AttributeVector* _attrs, Attribute* _attr);

typedef struct IXMLNode{
    char* tag;
    char* text;
    struct IXMLNode* daddy;
    AttributeVector attributes;
}XMLNode;

XMLNode* initXMLNode(XMLNode* _daddy);
void freeXMLNode(XMLNode* _node);

typedef struct IXMLDocumet{
    XMLNode * root;
}XMLDocument;


bool XMLDoc_load(XMLDocument* document,const char* path );
void XMLDoc_free(XMLDocument* document);




//______________________________________________________

void freeAttribute(Attribute* _attribute)
{
    free(_attribute->key);
    free(_attribute->value);
}

void InitAttributes(AttributeVector* _attrs)
{
    _attrs->heap_size = 1;
    _attrs->size=  0;
    _attrs->data = (Attribute*) malloc(sizeof(Attribute)* _attrs->heap_size);
}
void AddAttribute(AttributeVector* _attrs, Attribute* _attr)
{
    while (_attrs->size>= _attrs->heap_size) {
        _attrs->heap_size += 2;
        _attrs->data = (Attribute *) realloc(_attrs->data, sizeof(Attribute) * _attrs->heap_size);
    }
    _attrs->data[_attrs->size] = *_attr;
    _attrs->size++;
}



XMLNode* initXMLNode(XMLNode* _daddy)
{
    XMLNode * node = (XMLNode*) malloc(sizeof(XMLNode));
    node->daddy = _daddy;
    node->tag = NULL;
    node->text = NULL;
    //InitAttributes(&_daddy->attributes);

    _daddy->attributes.heap_size = 1;
    _daddy->attributes.size=  0;
    _daddy->attributes.data = (Attribute*) malloc(sizeof(Attribute)* _daddy->attributes.heap_size);
    return node;
}

void freeXMLNode(XMLNode* _node)
{
    if(_node->tag) free(_node->tag);

    if(_node->text) free(_node->text);
    for (int i = 0; i < _node->attributes.size; ++i) {
        freeAttribute(&_node->attributes.data[i]);
    }

    free(_node);

}


bool XMLDoc_load(XMLDocument* document,const char* path )
{
    FILE* fp = fopen(path,"r");
    if(!fp){
        printf("cant open lol");
        return false;
    }
    //get fiel size
    fseek(fp,  0,  SEEK_END);
    int size = ftell(fp);
    fseek(fp,0,SEEK_SET);

    char* buffer = (char *) malloc(sizeof (char) * size + 1);
    fread(buffer,1, size, fp);
    fclose(fp);
    buffer[size] = '\0';

    document->root = initXMLNode(NULL);

    //lexical analsis
    char lexical_buffer[265];
    int lex_pointer = 0;
    int i = 0;

    XMLNode * current = NULL;
    while (buffer[i] != '\0')
    {
        if(buffer[i] == '<')
        {
            //inner text
            lex_pointer[lexical_buffer] = '\0';
            if(strlen(lexical_buffer) > 0)
            {
                if(!current)
                {
                    printf("text outside doc");
                    return false;
                }
                current->text = strdup(lexical_buffer);
                lex_pointer = 0;
            }

            //end of node
            if(buffer[i+1] == '/')
            {
                i+=2;
                while (buffer[i] != '>')
                {
                    lexical_buffer[lex_pointer] = buffer[i];
                    i++;
                    lex_pointer++;
                }
                lexical_buffer[lex_pointer] = '\0';
                if(!current)
                {
                    printf("didnt read opening before");
                    return false;
                }
                if(strcmp(current->tag, lexical_buffer))
                {
                    printf("current not same as lexical buffet\n %s %s", current->tag, lexical_buffer);
                    return false;
                }
                current = current->daddy;
                i++;
                continue;
            }

            //set current node
            if(!current) current = document->root;
            else current = initXMLNode(current);
            i++;
            //get tag name

            while (buffer[i] != '>')
            {
                lex_pointer[lexical_buffer] = buffer[i]; //magic ;)
                i++;
                lex_pointer++;
            }
            lex_pointer[lexical_buffer] = '\0';
            current->tag = strdup(lexical_buffer);
            //reset tis
            lex_pointer = 0;
            i++;
            continue;
        }else{
            lex_pointer[lexical_buffer] = i[buffer];
            i++;
            lex_pointer++;

        }
    }

    return true;
}


void XMLDoc_free(XMLDocument* document)
{
    free(document->root);
}

#endif //XML_PARSER_SPEEDRUN_XML_PARSER_H
