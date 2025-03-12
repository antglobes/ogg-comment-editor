#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h> 


char inFileName[MAX_PATH] = "";

#if UINT_MAX >= 0xffffffff
typedef unsigned int crc32_t;
#else
typedef unsigned long crc32_t;
#endif

typedef unsigned char      byte;    // Byte is a char

typedef unsigned short int word16;  // 16-bit word is a short int

typedef crc32_t            word32;  // 32-bit word is an int


static word32 crc_table[]=       // Table of 8-bit remainders

{
0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005,
0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75,
0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD,
0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039, 0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5,
0xBE2B5B58, 0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D,
0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D,
0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072,
0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16, 0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA,
0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02,
0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692,
0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A,
0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E, 0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2,
0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34, 0xDC3ABDED, 0xD8FBA05A,
0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53,
0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B,
0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF, 0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623,
0xF12F560E, 0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B,
0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B,
0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3,
0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640, 0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C,
0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24,
0x119B4BE9, 0x155A565E, 0x18197087, 0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654,
0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C,
0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18, 0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4,
0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662, 0x933EB0BB, 0x97FFAD0C,
0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668, 0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4
};

word32 get_crc(word32 crc_accum, byte *data_blk_ptr, word32 data_blk_size)
{
   register word32 i, j;

   for (j=0; j<data_blk_size; j++)
   {
     i = ((int) (crc_accum >> 24) ^ *data_blk_ptr++) & 0xFF;
     crc_accum = (crc_accum << 8) ^ crc_table[i];
   }
   crc_accum = ~crc_accum;

   return crc_accum^0xFFFFFFFF;
}

#define PART_HEADER_SIZE 0x1B

#define CONTINUED_FLAG  0x1
#define BoS_FLAG    0x2
#define EoS_FLAG    0x4

#define CharsToInt32(table,offset)  (table[offset]+(table[offset+1]<<8)+(table[offset+2]<<16)+(table[offset+3]<<24))
#define CharsToInt64H(table,offset) (CharsToInt32(table,offset+4))
#define CharsToInt64L(table,offset) (CharsToInt32(table,offset))

int   i_sndtype,
        i_quality;

float //Stlk quality,

    f_mindist,
    f_maxdist,
    f_maxaidist,
    f_basevol;

unsigned char ogg_corr[]={0x03,0x00,0x00,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,0xC0,0x41,0x00,0x00,0x80,0x3F,0x80,0x00,0x00,0x08,0x00,0x00,0xC0,0x41};

//unsigned char ogg_com_header[8]; //HEX DUMP ogg-comment-header

unsigned char ogg_com_header[]={0x01,0x00,0x00,0x00,0x18,0x00,0x00,0x00};
        

__declspec( dllexport ) int oggnav( char* fname ,int mod ){
    // fname -filename .ogg
    // mod -0 -check/view
    
    
    // mod -2 -add ogg-comment
    
    
    
      FILE* file=NULL;
      FILE* file_new=NULL;
    
      crc32_t crc32;
      crc32_t crc32_file;
      unsigned char part_header[PART_HEADER_SIZE]; /*used to read the header (not including Segment_Table)*/
      unsigned char *segment_table=NULL; /*Used to store segment_table, the size depends on the page_segments*/
      const unsigned char capture_pattern[4]={0x4f, 0x67, 0x67, 0x53}; /*page title signature, 'oggs'*/
      int page_start=0;      /*used to store the beginning of the "current" (processed) page*/
      int page_start_prev=0; /*used to store the beginning of the "previous" page*/
      int  page_data_size;    /*Used to store page data size*/
      int c; /*meter in segment_table processing cycles*/
      int stream_counter=0; /*calculation of the total number of logical flows in the file*/
      int page_counter=0; /*Calculation of the total number of pages in the file*/
      unsigned char *databuf=0; /*Boofer for these page*/
      char fname_new[264];
    
         int comment_flag=0; //If the block contains a comment, we process through the buffer
    
    
      int offset=0;/*used to maintain the current displacement when displaying comments*/
        int base=0, comment_base=0;
    
    
    
    //_test_("MODE :[%d]\n\n",mod);
    
    
      file = fopen( fname, "rb+" );
      if( !file ){
        printf( "!!! Error opening file: %s", fname );
        return -1;
      }
    
      if (mod==2)
      { memset(&fname_new, 0, sizeof(fname_new));
        strcpy(fname_new,fname);
        strcat(fname_new,"~.tmp");
        file_new = fopen( fname_new, "wb" );
        if( !file_new ){
          printf( "!!! Error opening new_file: %s", fname_new );
          fclose( file );
          return -1;
        }
    
      }
    
      while( fread( part_header, 1, sizeof(part_header), file ) )
      {
        crc32 = 0x00000000;
        if( memcmp( part_header, capture_pattern, 4 ) != 0 ){/*verification for a coincidence with the signature, in case of mismatch, completion of work*/
          printf( "!!! --- sync lost at offset %d (0x%X), must be 'OggS', is '%c%c%c%c' ---",
            page_start, page_start, part_header[0], part_header[1], part_header[2], part_header[3] );
          if( segment_table ) free( segment_table );
          fclose( file );
          if(file_new != NULL) fclose( file_new );
          return -1;
        }
        if( part_header[4] != 0 ){
          printf( "!!! --- unknown version of page header structure, must be 0, is %d ---", part_header[5] );
          if( segment_table ) free( segment_table );
          fclose( file );
          if(file_new != NULL) fclose( file_new );
          return -1;
        }
    
        /*ok, page title was found, the version was checked*/
        page_counter++;
        memcpy(&crc32_file,part_header+22,4);
    
    //    if ((CharsToInt32(part_header,18))<2) ViewData=1; else  ViewData=0;
    
    
        part_header[22]=0; part_header[23]=0; part_header[24]=0; part_header[25]=0; //Knock the CRC32 of the segment
    
    
            /*Now we read Segment_Table*/
        if( part_header[26] == 0 )
        {
              continue;
        }
        segment_table =malloc( (int)part_header[26] );
        if( !segment_table )
        {
          printf( "!!! !!! Error allocating %d bytes of memory. Program terminating", part_header[26] );
          fclose( file );
          if(file_new != NULL) fclose( file_new );
          return -1;
        }
        if( !fread( segment_table, 1, (int)part_header[26], file ) )
        {
          if( segment_table ) free( segment_table );
          fclose( file );
          if(file_new != NULL) fclose( file_new );
          printf( "!!! --- Error reading segment_table (%d bytes), offset:%d (0x%d) ---", part_header[26], (int)ftell(file), (int)ftell(file) );
          return -1;
        }
    
        /*We process segment_table*/
        page_data_size = 0;
        for( c=0; c<part_header[26]; c++ )
        {
          page_data_size += segment_table[c];
        }
    
        databuf = malloc( page_data_size );
        if( !databuf ){
          printf("!!! !!! Error allocating %d bytes of memory. Program terminating", page_data_size);
          if(segment_table) free(segment_table);
          fclose(file);
          if(file_new != NULL) fclose( file_new );
          return -1;
        }
        if(!fread(databuf,1,page_data_size,file)){
          printf("!!! --- Error reading page content");
          if(segment_table) free(segment_table);
          fclose(file);
          if(file_new != NULL) fclose( file_new );
          return -1;
        }
    
                        if(!(!databuf || (page_data_size<8) || (part_header[5]&CONTINUED_FLAG))) /*Ignore possible errors when transmitting parameters*/
                        {
    
                        if (databuf[0]==0x03) // Found a block with a comment
    
                            {
    //_test_("comments offset (0x%X)\n",part_header[26]+PART_HEADER_SIZE);
    
                                comment_flag=1;
                          comment_base=11+CharsToInt32(databuf,7); //"Vorbis"+4+displacement at the beginning of the comment text, taking into account "Creed Using"
    
                          offset=comment_base+4;
    //_test_("numbers of comments:[%d] offset[%d](0x%X) L:%d\n",CharsToInt32(databuf,comment_base),comment_base,comment_base,__LINE__);
    //_test_("comment_base offset (0x%X)\n",comment_base+part_header[26]+PART_HEADER_SIZE);
    
    
                          for(c=0;c<CharsToInt32(databuf,comment_base);c++)
                          {
                            offset+=4+CharsToInt32(databuf,offset);
    //_test_("comment #%d offset[%d](0x%X) L:%d\n",c+1,offset,offset,__LINE__);
    //_test_("comment  offset (0x%X)\n",offset+part_header[26]+PART_HEADER_SIZE);
    
                          }
    //_test_("total comment length [%d] (0x%X)\n",offset-comment_base,offset-comment_base);
    //_test_("32-offset [%d]\n",comment_base+32-offset);
    
                            }
                            else
                                comment_flag=0;
                        }
    
      if ((mod==0) && (comment_flag))
      {
        if ((CharsToInt32(databuf,comment_base)==1) && ((offset-comment_base)==32)) //comments 1 and its length = 32 bytes -suitable for editing
    
        {
    
          c=comment_base+8;
    //_test_("%02X %02X %02X %02X \n",ogg_h[i],ogg_h[i+1],ogg_h[i+2],ogg_h[i+3]);
    
          memcpy(&i_quality,databuf+c,4); c+=4;
    //_test_("%02X %02X %02X %02X \n",ogg_h[i],ogg_h[i+1],ogg_h[i+2],ogg_h[i+3]);
    
          memcpy(&f_mindist,databuf+c,4); c+=4;
    //_test_("%02X %02X %02X %02X \n",ogg_h[i],ogg_h[i+1],ogg_h[i+2],ogg_h[i+3]);
    
          memcpy(&f_maxdist,databuf+c,4); c+=4;
    //_test_("%02X %02X %02X %02X \n",ogg_h[i],ogg_h[i+1],ogg_h[i+2],ogg_h[i+3]);
    
          memcpy(&f_basevol,databuf+c,4); c+=4;
    //_test_("%02X %02X %02X %02X \n",ogg_h[i],ogg_h[i+1],ogg_h[i+2],ogg_h[i+3]);
    
          memcpy(&i_sndtype,databuf+c,4); c+=4;
    //_test_("%02X %02X %02X %02X \n",ogg_h[i],ogg_h[i+1],ogg_h[i+2],ogg_h[i+3]);
    
          memcpy(&f_maxaidist,databuf+c,4); c+=4;
    //_test_("seek: %#X , MaxAIdist: %#X (%f) \n",i,stlk_maxaidist,stlk_maxaidist);
    
    
    
    // IDC_Edit_mindist 1011
    // IDC_Edit_maxdist 1012
    // IDC_Edit_maxaidist 1013
    // IDC_Edit_basevol 1014
    
    
            //Strcpy(txt sndtype,"\0");
    
    
           switch(i_sndtype)
           {
            case 134217856: c=20;               //"World ambient"
    
                  break;
            case 134217984: c=19;               //"Object exploding"
    
                  break;
            case 134218240: c=18;               //"Object colliding"
    
                  break;
            case 134218752: c=17;               //"Object breaking"
    
                  break;
            case 268437504: c=16;               //"Anomaly idle"
    
                  break;
            case 536875008: c=15;               //"NPC eating"
    
                  break;
            case 536879104: c=14;               //"NPC attacking"
    
                  break;
            case 536887296: c=13;               //"NPC talking"
    
                  break;
            case 536903680: c=12;               //"NPC step"
    
                  break;
            case 536936448: c=11;               //"NPC injuring"
    
                  break;
            case 537001984: c=10;               //"NPC dying"
    
                  break;
            case 1077936128: c=5;               //"Item using"
    
                  break;
            case 1082130432: c=3;               //"Item taking"
    
                  break;
            case 1090519040: c=4;               //"Item hiding"
    
                  break;
            case 1107296256: c=2;               //"Item dropping"
    
                  break;
            case 1140850688: c=1;               //"Item picking up"
    
                  break;
            case 2147745792: c=9;               //"weapon recharging"
    
                  break;
            case 2148007936: c=8;               //"Weapon bullet hit"
    
                  break;
            case 2148532224: c=7;               //"Weapon empty clicking"
    
                  break;
            case 2149580800: c=6;               //"Weapon shooting"
    
                  break;
    
            default:  c=0;                      //"undefined"
    
                  break;
           }
    
    
    
           if (i_quality==3) //It is not yet clear
    
           {
             //Ret+=2;
    
              printf("Good to edit.");
           }
           else
           {
              printf("Warning! Not legal ogg-comment !!!");
           }
    
        }
        else
        {
           printf("No ogg-comment or invalid version.");
        }
      }
    
        if (mod==2)
        {
    
    // you need to calculate the size of the comment, new
    // And the old
    /*
    VOID PAGE_DATA_PROCESS (UNSIGned Char*Buf, Int Size, Char Flags, Intric_num)
    The function processes the pages of headings and comments, displaying them Hex-DUMP and decryption.
    Все The remaining pages (not headlines and not comments) are ignored*/
    
                if (comment_flag)
                {
    
                  unsigned char *newdatabuf=0; /*Boofer for these page*/
                int newst=comment_base+32-offset;
    
                newdatabuf = malloc( page_data_size+part_header[26]+PART_HEADER_SIZE+32);
    
                base=0;
            //Copy the title
    
                memcpy(newdatabuf,part_header,sizeof(part_header));
                base+=sizeof(part_header);
    
            if ((comment_base-offset)!=32) //If the size of the comment block! = 32 (28 + 4)
    
            {
              for( c=0; c<part_header[26]; c++ )//Recalculate segment_table
    
              {
                if ((((int)segment_table[c]+newst)>0) && (((int)segment_table[c]+newst)<256))
                {
                    segment_table[c]+=newst;
                    break;
                }
                else
                {
                    if (c<(part_header[26]-1))
                    {
                      segment_table[c]=0xFF;
                      newst-=segment_table[c+1];
                    }
                    else
                        if (newst<256)
                      {
                          segment_table[c]=newst;
                      }
                      else
                      {  printf("!!! --- Error set segment_table");
                       fclose(file);
                       if(file_new != NULL) fclose( file_new );
                       return -1;
                      }
                }
              }
            }
    
    //Comment Base -the beginning of the comment block (including the number of comments)
    //Offset -end of the comment block
    
    
            //Copy the segments block
    
                memcpy(newdatabuf+base,segment_table,(int)part_header[26]);
                base+=(int)part_header[26];
    
    
            //Copy the beginning of the data block (without comment)
    
                memcpy(newdatabuf+base,databuf,comment_base);
                base+=comment_base;
    
            //Insert the headline comment
    
                memcpy(newdatabuf+base,ogg_com_header,8);
                base+=8;
    
            //We insert your comment
    
                memcpy(newdatabuf+base,ogg_corr,24);
                base+=24;
    
            //Copy the remaining data
    
                memcpy(newdatabuf+base,databuf+offset,page_data_size-offset);
                base+=page_data_size-offset;
    
            crc32 = get_crc(crc32, newdatabuf, base); //Recalculate CRC32
    
            memcpy(newdatabuf+22,&crc32,4);
    
            c=fwrite(newdatabuf, 1, base, file_new);
    //_test_("writ ret:%d L:%d\n",ret,__LINE__);
    
            if (c!=base)
            {
              printf( " Write - FAILED L:%d!",__LINE__);
              fclose( file );
              fclose( file_new );
              free(newdatabuf);
              return -1;
            }
            free(newdatabuf);
          }//if comments block
    
          else
          {//if no comments
    
    
            //#!#!#!#!#!#!#!#!#
    
    
            memcpy(part_header+22,&crc32_file,4);
    
            c=fwrite(part_header, 1, sizeof(part_header), file_new);
    //_test_("writ ret:%d L:%d\n",ret,__LINE__);
    
            if (c!=sizeof(part_header))
            {
              printf( " Write - FAILED L:%d!",__LINE__);
              fclose( file );
              fclose( file_new );
              return -1;
            }
    
            //#!#!#!#!#!#!#!#!#
    
            c=fwrite(segment_table, 1, (int)part_header[26], file_new);
    //_test_("writ ret:%d L:%d\n",ret,__LINE__);
    
            if (c!=(int)part_header[26])
            {
              printf( " Write - FAILED L:%d!",__LINE__);
              fclose( file );
              fclose( file_new );
              return -1;
            }
    
            //#!#!#!#!#!#!#!#!#
    
            c=fwrite(databuf, 1, page_data_size, file_new);
    //_test_("writ ret:%d L:%d\n",ret,__LINE__);
    
            if (c!=page_data_size)
            {
              printf( " Write - FAILED L:%d!",__LINE__);
              fclose( file );
              fclose( file_new );
              return -1;
            }
          }//if no comments
    
        }
        /*We skip the segment data, so as to read (in the next cycle iteration) heading of the next segment*/
        free( segment_table );
        segment_table = NULL;
    
        free(databuf);
        databuf = NULL;
    
      }
    
      if(file != NULL)
      {
        fclose( file );
        file = NULL;
      }
    
      if(file_new != NULL)
      {
        fclose( file_new );
        file_new = NULL;
      }
    
      if (mod==2)
      {
    
       if(remove(fname) != 0)
       {
          printf("!!! --- Error: unable to delete origin file.");
              return -1;
       }
    
       if(rename(fname_new, fname) != 0)
       {
          printf("!!! --- Error: unable to rename temp file.");
              return -1;
       }
      }
    
    
      return 0;
    }
    
__declspec( dllexport) int amp_AddOggComment(char* path, float mindist, float maxdist, float basevol, int sndtype, float maxaidist){
    int quality = 3;

    memcpy(ogg_corr,&quality,4);
    memcpy(ogg_corr+4,&mindist,4);
    memcpy(ogg_corr+8,&maxdist,4);
    memcpy(ogg_corr+12,&basevol,4);
    memcpy(ogg_corr+16,&sndtype,4);
    memcpy(ogg_corr+20,&maxaidist,4);

    if (oggnav(path, 2) < 0){
        printf("Error Processing");
        return -1;
    }
    oggnav(path, 0);


    return 0;
}