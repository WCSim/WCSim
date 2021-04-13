#include <TSystem.h> 
#include <string.h>
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"
#include <TROOT.h>

// Compile Me
// g++ -o hadd_wcsim.exe hadd_wcsim_compileMe.C `root-config --cflags --libs`

using namespace std;
 
TList *FileList;
TFile *Target;
 
void MergeRootfile( TDirectory *target, TList *sourcelist );
void hadd(const char* list, const char* out);


int main(int argc , char* argv[]){

  const char* list;
  const char* out;
  if( argc<3 ){  
    std::cout<<" Usage:  ./hadd_wcsim.exe  wcsim.list  wcsim_hadd.root " << std::endl;
    std::cout<<" Should provide names of input list and output ROOT file" << std::endl; 
    std::cout<<" Using default names  wcsim.list and wcsim_hadd.root" << std::endl;
  }
  else{
    list = argv[1];  std::cout<<"Input list: "<< list << std::endl;
    out = argv[2];  std::cout<<"Output ROOT file:: "<< out << std::endl;
  }  
  gROOT->Reset();
  char* wcsimdirenv;
  wcsimdirenv = getenv ("WCSIMDIR");
  if(wcsimdirenv !=  NULL){
    gSystem->Load("${WCSIMDIR}/libWCSimRoot.so");
  } 
  else{
    cout<<"Must set WCSIMDIR" << std::endl;
  }

  std::cout<<"Run hadd function"<< std::endl;
  hadd(list, out);
  std::cout<<"Completed hadd function"<< std::endl;
}
 

void hadd(const char* list, const char* out){

   Target = TFile::Open( out , "RECREATE" );
 
   FileList = new TList();  
   // Reading a list to add to RList
   std::ifstream w_file( list );
   if (w_file.is_open()) {
       std::string w_line;
       while (getline(w_file, w_line)) {
         FileList->Add( TFile::Open( w_line.c_str() ));
       }
       w_file.close();
   }
   MergeRootfile( Target, FileList );
}
 

void MergeRootfile( TDirectory *target, TList *sourcelist ) {
 
   //  cout << "Target path: " << target->GetPath() << endl;
   TString path( (char*)strstr( target->GetPath(), ":" ) );
   path.Remove( 0, 2 );
 
   TFile *first_source = (TFile*)sourcelist->First();
   first_source->cd( path );
   TDirectory *current_sourcedir = gDirectory;
   //gain time, do not add the objects in the list in memory
   Bool_t status = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
 
   // loop over all keys in this directory
   TChain *globChain = 0;
   TIter nextkey( current_sourcedir->GetListOfKeys() );
   TKey *key, *oldkey=0;
   while ( (key = (TKey*)nextkey())) {
 
      //keep only the highest cycle number for each key
      if (oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;
 
      // read object from first source file
      first_source->cd( path );
      TObject *obj = key->ReadObj();

      if ( obj->IsA()->InheritsFrom( TH1::Class() ) ) {
         // descendant of TH1 -> merge it
 
         //      cout << "Merging histogram " << obj->GetName() << endl;
         TH1 *h1 = (TH1*)obj;
 
         // loop over all source files and add the content of the
         // correspondant histogram to the one pointed to by "h1"
         TFile *nextsource = (TFile*)sourcelist->After( first_source );
         while ( nextsource ) {
 
            // make sure we are at the correct directory level by cd'ing to path
            nextsource->cd( path );
            TKey *key2 = (TKey*)gDirectory->GetListOfKeys()->FindObject(h1->GetName());
            if (key2) {
               TH1 *h2 = (TH1*)key2->ReadObj();
               h1->Add( h2 );
               delete h2;
            }
 
            nextsource = (TFile*)sourcelist->After( nextsource );
         }
      }
      else if ( obj->IsA()->InheritsFrom( TTree::Class() ) ) {
 
         // loop over all source files create a chain of Trees "globChain"
         const char* obj_name= obj->GetName();
 
         globChain = new TChain(obj_name);
         globChain->Add(first_source->GetName());
         TFile *nextsource = (TFile*)sourcelist->After( first_source );
         //      const char* file_name = nextsource->GetName();
         // cout << "file name  " << file_name << endl;
         while ( nextsource ) {
 
            globChain->Add(nextsource->GetName());
            nextsource = (TFile*)sourcelist->After( nextsource );
         }
 
      } else if ( obj->IsA()->InheritsFrom( TDirectory::Class() ) ) {
         // it's a subdirectory
 
         cout << "Found subdirectory " << obj->GetName() << endl;
 
         // create a new subdir of same name and title in the target file
         target->cd();
         TDirectory *newdir = target->mkdir( obj->GetName(), obj->GetTitle() );
 
         // newdir is now the starting point of another round of merging
         // newdir still knows its depth within the target file via
         // GetPath(), so we can still figure out where we are in the recursion
         MergeRootfile( newdir, sourcelist );
 
      } else {
 
         // object is of no type that we know or can handle
         cout << "Unknown object type, name: "
         << obj->GetName() << " title: " << obj->GetTitle() << endl;
      }
 
      // now write the merged histogram (which is "in" obj) to the target file
      // note that this will just store obj in the current directory level,
      // which is not persistent until the complete directory itself is stored
      // by "target->Write()" below
      if ( obj ) {
         target->cd();
 
         //!!if the object is a tree, it is stored in globChain...
         if(obj->IsA()->InheritsFrom( TTree::Class() ))
            globChain->Merge(target->GetFile(),0,"keep");
         else
            obj->Write( key->GetName() );
      }
 
   } // while ( ( TKey *key = (TKey*)nextkey() ) )
 
   // save modifications to target file
   target->SaveSelf(kTRUE);
   TH1::AddDirectory(status);

}
