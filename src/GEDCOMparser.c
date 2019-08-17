#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "GEDCOMparser.h"
#include "LinkedListAPI.h"
#include "readfile.h"
int setSign(char p){
    if(p==' '||p=='\n'){
        return 0;
    }else{
        return 1;
    }
}
int* checkPos(char *p,int *record){
    int i=0,z=0;
    z=strlen(p);
    for(i=0;i<z;i++){
        if(*(p+i) == '/'){
            record[0] = i+1;
            break;
        }
    }
    for(i=record[0];i<z;i++){
        if(p[i]=='/'){
            record[1]=i+1;
            break;
        }
    }
    return record;
}

GEDCOMerror createGEDCOM(char* fileName, GEDCOMobject** obj)
{
    GEDCOMerror error;

    FILE *fp=fopen(fileName,"r");
    if(obj==NULL)
    return error;
    if(fp==NULL){
        error.type=INV_FILE;
        error.line=-1;
        printf("File can not be opened, since NULL\n");
        return error;

    }
    char temp[100];
    int line=0,i=0,z=0,w=0,eventNumber=0,s=0,h=0,p=0,f=0;
    int *record=malloc(sizeof(int)*2);
    while(fgets(temp,100, fp)!=NULL){
         line++;
    }
    //printf("%d",line);
    if(line==0){
        error.type=INV_FILE;
        error.line=-1;
        printf("The file is Empty\n");
        return error;

    }
    //obj=malloc(1000);
    Individual *indi=malloc(sizeof(Individual)*line);
    Event *event=malloc(sizeof(Event)*line);
    Submitter *submitter=malloc(sizeof(submitter));
    Header *header=malloc(sizeof(Header));
    header->submitter=malloc(100);
    Family *family=malloc(sizeof(Family)*line);

    for(i=0;i<line;i++){
    (indi+i)->givenName=malloc(100);
    //memcpy((indi+i)->givenName,0, sizeof((indi+i)->givenName));
    (indi+i)->surname=malloc(100);
    (family+i)->husband=malloc(100);
    (family+i)->wife=malloc(100);
    (event+i)->date=malloc(100);
    (event+i)->place=malloc(100);

    //(header+i)->submitter=malloc(100);
    }
    obj[0]->families=initializeList(&printFamily,&deleteFamily,&compareFamilies);
    obj[0]->individuals=initializeList(&printIndividual,&deleteIndividual,&compareIndividuals);
    obj[0]->header=header;
    obj[0]->submitter=submitter;

    int lineSize[line],firstBlankPos[line],secBlankPos[line],nextLine[line];
    int firstStart[line],secondStart[line],thirdStart[line];
    char tokenTag[line][20], tokenValue[line][100];
    char person[line][8];

    for(i=0;i<line;i++){
        lineSize[i]=0;firstBlankPos[i]=0;secBlankPos[i]=0;nextLine[line]=0;
    }

    Read *info=malloc(line*1000);

    read(fileName,info);

    for(i=0;i<line;i++){//determine the size of each line;
        lineSize[i]=strlen((info+i)->line);
        nextLine[i]=lineSize[i];
    }
    for(i=0;i<line;i++){
        for(z=0;z<lineSize[i];z++){
            if(setSign((info+i)->line[z])==1){
                firstStart[i]=z;
                break;
            }
        }
        for(z=firstStart[i];z<lineSize[i];z++){//specific position of first blank
            if(setSign((info+i)->line[z])==0){
                firstBlankPos[i]=z;
                break;
            }
        }

    }// count firstBlankPos firstStart//firstStart,firstBlankPos
    for(i=0;i<line;i++){
        for(z=firstBlankPos[i];z<lineSize[i];z++){
            if(setSign((info+i)->line[z])==1){
                secondStart[i]=z;
                break;
            }
        }
        for(z=secondStart[i];z<lineSize[i];z++){
            if(setSign((info+i)->line[z])==0){
                secBlankPos[i]=z;
                break;
            }
        }
    }//secondStart secBlankPos

    //count first start firstBlankPos
    if(secBlankPos[line-1]==0){
        secBlankPos[line-1]=strlen((info+line-1)->line);
    }
    //In case if no thirdStart
    for(i=0;i<line;i++){
        if((info+i)->line[secBlankPos[i]]=='\n'){
            thirdStart[i]=secBlankPos[i];
        }else if(lineSize[i]!=secBlankPos[i]){
            for(z=secBlankPos[i];z<lineSize[i];z++){
                if(setSign((info+i)->line[z])==1){
                    thirdStart[i]=z;
                    break;
                }
            }
        }else{
            thirdStart[i]=secBlankPos[i];
        }
    }//count thirdStart
    //count thirdStart
    for(i=0;i<line;i++){
        for(z=0;z<=lineSize[i];z++){
            if((info+i)->line[z]=='\n'){
                nextLine[i]=z;
                break;
            }
        }
    }
    //check the position of nextLine
    if(nextLine[line-1]!=lineSize[line-1]){
        nextLine[line-1]=lineSize[line-1];
    }
    //If no nextLine position
    //for(i=0;i<line;i++)
    //printf("%d: firstStart: %d firstBlankPos: %d secondStart: %d secBlankPos: %d thirdStart: %d nextLine: %d\n", i,firstStart[i],firstBlankPos[i],secondStart[i],secBlankPos[i],thirdStart[i],nextLine[i]);
    for(i=0;i<line;i++){
        strncpy(tokenTag[i], (info+i)->line+secondStart[i],secBlankPos[i]-secondStart[i]);
        strncpy(tokenValue[i],(info+i)->line+thirdStart[i],nextLine[i]-thirdStart[i]);
    }//start to paring

    if(memcmp(tokenTag[0],"HEAD",4)!=0){
        error.type=INV_FILE;
        error.line=-1;
        printf("The head is missing\n");
        return error;
    }
    if(memcmp(tokenTag[line-1],"TRLR",4)!=0){
        error.type=INV_FILE;
        error.line=-1;
        printf("The Tail is missing\n");
        return error;
    }
    for(i=0;i<line;i++){
        if((info+i)->line[firstStart[i]]=='0'&&memcmp(tokenTag[i], "HEAD", 4)==0){
            i++;

            while((info+i)->line[0]!='0'){
                //printf("%d",strlen(tokenTag[i]));
                //printf("%d",secBlankPos[i]-secondStart[i]);
                if(memcmp(tokenTag[i], "SOUR", 4)!=0&&memcmp(tokenTag[i], "VERS", 4)!=0
                &&memcmp(tokenTag[i], "NAME", 4)!=0&&memcmp(tokenTag[i], "FILE", 4)!=0
                &&memcmp(tokenTag[i], "GEDC", 4)!=0&&memcmp(tokenTag[i], "FORM", 4)!=0
                &&memcmp(tokenTag[i], "CHAR", 4)!=0&&memcmp(tokenTag[i], "SUBM", 4)!=0
                &&memcmp(tokenTag[i], "DEST", 4)!=0&&memcmp(tokenTag[i], "DATE", 4)!=0
                &&memcmp(tokenTag[i], "REFN", 4)!=0){
                    printf("%d",i);
                    printf("%s\n",tokenTag[i]);
                    error.type=INV_HEADER;
                    error.line=-1;
                    printf("The header is invalid since misspell\n");
                }
                if(memcmp(tokenTag[i], "SUBM", 4)==0){
                    if(memcmp(tokenValue[i], "\0", 1)==0){
                        printf("%d",i);
                        error.type=INV_HEADER;
                        error.line=-1;
                    }
                }
                i++;
            }
            i--;
        }
        if((info+i)->line[firstStart[i]]=='0'&&memcmp(tokenValue[i], "INDI", 4)==0){
            i++;
            while((info+i)->line[0]!='0'){
                if(memcmp(tokenTag[i], "NAME", 4)!=0&&memcmp(tokenTag[i], "GIVN", 4)!=0
                &&memcmp(tokenTag[i], "SURN", 4)!=0&&memcmp(tokenTag[i], "SEX", 3)!=0
                &&memcmp(tokenTag[i], "BIRT", 4)!=0&&memcmp(tokenTag[i], "CHR", 3)!=0
                &&memcmp(tokenTag[i], "DATE", 4)!=0&&memcmp(tokenTag[i], "DEAT", 4)!=0
                &&memcmp(tokenTag[i], "PLAC", 4)!=0&&memcmp(tokenTag[i], "BIRT", 4)!=0
                &&memcmp(tokenTag[i], "FAMS", 4)!=0&&memcmp(tokenTag[i], "BURI", 4)!=0
                &&memcmp(tokenTag[i], "FAMC", 4)!=0&&memcmp(tokenTag[i], "FAMS", 4)!=0
                &&memcmp(tokenTag[i], "TITL", 4)!=0&&memcmp(tokenTag[i], "REFN", 4)!=0
                &&memcmp(tokenTag[i], "OCCU", 4)!=0&&memcmp(tokenTag[i], "NOTE", 4)!=0
                &&memcmp(tokenTag[i], "CONT", 4)!=0&&memcmp(tokenTag[i], "QUAY", 4)!=0
                &&memcmp(tokenTag[i], "BAPM", 4)!=0){
                    printf("%d\n",i);
                    printf("%s",tokenTag[i]);
                    error.type=INV_RECORD;
                    error.line=-1;
                }
                i++;
            }
            i--;
        }


    }

    //read data into tokenTag and tokenValue
    for(i=0;i<line;i++){
        if((info+i)->line[firstStart[i]]=='0'&&memcmp(tokenTag[i], "HEAD", 4)==0){
            i++;
            while((info+i)->line[0]!='0'){
                if(memcmp(tokenTag[i], "SOUR", 4)==0){
                    memcpy((header)->source,tokenValue[i],sizeof((header)->source));

                }else if(memcmp(tokenTag[i], "VERS", 4)==0){
                    (header+h)->gedcVersion=atof(tokenValue[i]);

                }else if(memcmp(tokenTag[i], "CHAR", 4)==0){

                    CharSet c=ANSEL;
                    if(memcmp(tokenValue[i], "ANSEL", 5)==0){
                        c=ANSEL;
                    }else if(memcmp(tokenValue[i], "UTF", 3)==0){
                        c=UTF8;
                    }else if(memcmp(tokenValue[i], "UNICODE", 7)==0){
                        c=UNICODE;
                    }else if(memcmp(tokenValue[i], "ASCII", 5)==0){
                        c=ASCII;
                    }
                    (header+h)->encoding=c;

                }else{
                    //insertBack(&obj[0]->header->otherFields,tokenTag[i]);
                    //insertBack(&obj[0]->header->otherFields,tokenValue[i]);
                }
                i++;
            }
            i--;
        }
        if((info+i)->line[firstStart[i]]=='0'&&memcmp(tokenValue[i], "SUBM", 4)==0){
            i++;
            while((info+i)->line[0]!='0'){
                if(memcmp(tokenTag[i], "NAME", 4)==0){
                    memcpy((header->submitter+s)->submitterName,tokenValue[i],sizeof((header->submitter+s)->submitterName));
                }else if(memcmp(tokenTag[i], "ADDR", 4)==0){
                    strcpy((header->submitter+s)->address,tokenValue[i]);
                }else{
                    insertBack(&(header->submitter)->otherFields,tokenTag[i]);
                    insertBack(&(header->submitter)->otherFields,tokenValue[i]);
                }
                i++;
            }
            i--;
        }
        if((info+i)->line[firstStart[i]]=='0'&&memcmp(tokenValue[i], "INDI", 4)==0){
            memcpy(person[p],tokenTag[i],sizeof(person[p]));
            p++;
            i++;
            while((info+i)->line[0]!='0'){
                //memcpy(tokenValue[i],"0",sizeof((indi+w)->surname));
                //memcpy((indi+w)->givenName,"0",sizeof((indi+w)->givenName));
                if(memcmp(tokenTag[i], "NAME", 4)==0){
                    memcpy((indi+w)->surname,tokenValue[i]+checkPos(tokenValue[i],record)[0],checkPos(tokenValue[i],record)[1]-checkPos(tokenValue[i],record)[0]-1);
                    memcpy((indi+w)->givenName,tokenValue[i]+0,checkPos(tokenValue[i],record)[0]-1);

                }else if((info+i)->line[firstStart[i]]=='1'&&memcmp(tokenValue[i], "\0", 1)!=0&&memcmp(tokenTag[i], "FAM", 3)!=0){
                    insertBack(&(indi+w)->otherFields,tokenTag[i]);
                    insertBack(&(indi+w)->otherFields,tokenValue[i]);
                    //insertBack(&obj[0]->individuals,tokenTag[i]);
                    //insertBack(&obj[0]->individuals,tokenValue[i]);
                }
                if((info+i)->line[firstStart[i]]=='1'&&memcmp(tokenValue[i], "\0", 1)==0){//if event meet
                    if(memcmp(tokenTag[i], "\0", 1)!=0){
                    strcpy((event+eventNumber)->type,tokenTag[i]);
                    insertBack(&(indi+w)->events,(event+eventNumber)->type);
                    //insertBack(&obj[0]->individuals,tokenTag[i]);
                    }
                    i++;
                    while((info+i)->line[firstStart[i]]!='1'){
                        if(memcmp(tokenTag[i], "DATE", 4)==0){
                            if(memcmp(tokenValue[i], "\0", 1)!=0){
                            (event+eventNumber)->date = tokenValue[i];
                            insertBack(&(indi+w)->events,(event+eventNumber)->date);
                            //insertBack(&obj[0]->individuals,tokenTag[i]);
                            //insertBack(&obj[0]->individuals,tokenValue[i]);
                            }
                        }else if(memcmp(tokenTag[i], "PLAC", 4)==0){
                            if(memcmp(tokenValue[i], "\0", 1)!=0){
                            (event+eventNumber)->place = tokenValue[i];
                            insertBack(&(indi+w)->events,(event+eventNumber)->place);
                            //insertBack(&obj[0]->individuals,tokenTag[i]);
                            //insertBack(&obj[0]->individuals,tokenValue[i]);
                            }
                        }else{
                            //insertBack(&obj[0]->individuals,tokenTag[i]);
                            //insertBack(&obj[0]->individuals,tokenValue[i]);
                            //insertBack(&(event+eventNumber)->otherFields=tokenTag[i]);
                            //insertBack(&(event+eventNumber)->otherFields=tokenValue[i]);
                        }
                        eventNumber++;
                        i++;
                    }

                    //eventNumbers[w-1]=eventNumber;
                    i--;
                }
                if((info+i)->line[firstStart[i]]=='1'&&memcmp(tokenTag[i], "FAMS", 3)==0){
                    //insertBack(&obj[0]->individuals,tokenTag[i]);
                    //insertBack(&obj[0]->individuals,tokenValue[i]);
                }
                i++;
            }
            //insertBack(&obj[0]->individuals,"");
            //printf("%s\n",(indi+w)->givenName);
            insertBack(&obj[0]->individuals,(indi+w));
            w++;
            i--;

        }
        if((info+i)->line[firstStart[i]]=='0'&&memcmp(tokenValue[i], "FAM", 3)==0){
            i++;
            while((info+i)->line[0]!='0'){
                if(memcmp(tokenTag[i], "HUSB", 4)==0){
                    for(z=0;z<p;z++){
                        if(memcmp(person[z], tokenValue[i], strlen(person[z]))==0){
                        (family+f)->husband=(indi+z);
                        //insertBack(&obj[0]->families,tokenTag[i]);
                        //insertBack(&obj[0]->families,(family+f)->husband)
                        //insertBack(&obj[0]->families,(family+f)->husband->surname);
                        }
                    }
                }else
                if(memcmp(tokenTag[i], "WIFE", 4)==0){
                    for(z=0;z<p;z++){
                        if(memcmp(person[z], tokenValue[i], strlen(person[z]))==0){
                        (family+f)->wife=(indi+z);
                        //insertBack(&obj[0]->families,tokenTag[i]);
                        //insertBack(&obj[0]->families,(family+f)->wife->givenName);
                        //insertBack(&obj[0]->families,(family+f)->wife->surname);
                        }

                    }
                }else
                if(memcmp(tokenTag[i], "CHIL", 4)==0){
                    for(z=0;z<p;z++){
                        if(memcmp(person[z], tokenValue[i], strlen(person[z]))==0){
                        insertBack(&(family+f)->children,strcat((indi+z)->givenName,(indi+z)->surname));
                        // insertBack(&obj[0]->families,tokenTag[i]);
                        // insertBack(&obj[0]->families,(indi+z)->givenName);
                        // insertBack(&obj[0]->families,(indi+z)->surname);
                        }
                    }
                }else {
                    insertBack(&(family+f)->otherFields,tokenTag[i]);
                    insertBack(&(family+f)->otherFields,tokenValue[i]);
                    // insertBack(&obj[0]->families,tokenTag[i]);
                    // insertBack(&obj[0]->families,tokenValue[i]);

                }
                i++;
            }
            insertBack(&obj[0]->families,(family+f));
            //insertBack(&obj[0]->families,"");
            f++;
            i--;
        }
    }

    return error;
}


char* printGEDCOM(const GEDCOMobject* obj){
    if(obj==NULL){
        return NULL;
    }
     toString(obj->families);
     toString(obj->individuals);
    return NULL;
}
void deleteGEDCOM(GEDCOMobject* obj){

}
char* printError(GEDCOMerror err){
    printf("%d",err.type);
    printf("%d",err.line);
    return NULL;
}
Individual* findPerson(const GEDCOMobject* familyRecord, bool (*compare)(const void* first, const void* second), const void* person){
    if(familyRecord==NULL||person==NULL)
    return NULL;
    return findElement(familyRecord->individuals,compare,person);

}
List getDescendants(const GEDCOMobject* familyRecord, const Individual* person){
    List list;
    return list;
}
void deleteFamily(void* toBeDeleted){
    Family* d_family;
    if (toBeDeleted == NULL){
        return;
    }
    d_family = (Family*)toBeDeleted;

    free(d_family->wife);
    free(d_family->husband);
    //free(d_family->children);
    //free(d_family->otherFields);
    free(d_family);
}
int compareFamilies(const void* first,const void* second){
    Family* family1;
	Family* family2;

	if (first == NULL || second == NULL){
		return 0;
	}

	family1 = (Family*)first;
	family2 = (Family*)second;
    return strcmp((char*)family1->husband, (char*)family2->husband);
}
char* printFamily(void* toBePrinted){
    char* tmpStr;
	Family* family;
	//int len;
    //printf("Y");
	if (toBePrinted == NULL){
		return NULL;
	}

	family = (Family*)toBePrinted;
	//len = strlen(family->husband->givenName)+strlen(family->husband->surname)+28;
	tmpStr = (char*)malloc(sizeof(char)*1000);
	sprintf(tmpStr, "Husband: %s %s\nWife: %s %s", family->husband->givenName, family->husband->surname,family->wife->givenName,family->wife->surname);
    printf("Family \n");
    printf("%s\n",tmpStr);
     while(family->children.head!=NULL){
         printf("Children: %s\n",(char *)family->children.head->data);
         family->children.head=family->children.head->next;
     }
    printf("\n");
    //free(tmpStr);
	return tmpStr;
}
void deleteIndividual(void* toBeDeleted){
    Individual* individual;
    if (toBeDeleted == NULL){
        return;
    }
    individual = (Individual*)toBeDeleted;

    free(individual->givenName);
    free(individual->surname);
    //free(d_family->children);
    //free(d_family->otherFields);
    free(individual);
}
int compareIndividuals(const void* first,const void* second){
    Individual* individual1;
	Individual* individual2;

	if (first == NULL || second == NULL){
		return 0;
	}

	individual1 = (Individual*)first;
	individual2 = (Individual*)second;
    return strcmp((char*)individual1->givenName, (char*)individual2->givenName);
}
char* printIndividual(void* toBePrinted){
    char* tmpStr;
	Individual* individual;
	//int len;
	if (toBePrinted == NULL){
		return NULL;
	}

	individual = (Individual*)toBePrinted;
	//len = strlen(individual->givenName)+strlen(individual->surname)+28;
	tmpStr = (char*)malloc(sizeof(char)*100);
	sprintf(tmpStr, "%s %s", individual->givenName, individual->surname);
    printf("Individual \n");
    printf("%s\n",tmpStr);
     while(individual->events.head!=NULL){
         printf("Events: %s\n",(char*)individual->events.head->data);
         individual->events.head=individual->events.head->next;
     }
     printf("\n");
	return tmpStr;
}
void deleteField(void* toBeDeleted){
}
int compareFields(const void* first,const void* second){
    return 0;
}
char* printField(void* toBePrinted){
    return NULL;
}
