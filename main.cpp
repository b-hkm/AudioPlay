
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <thread>

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)pInput;
}
char c;
void press();
int main(int argc, char** argv)
{
    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;
    std::string file;
   

    if (argc < 2) 
    {
        std::cout<<"Current DIR: "<<system("pwd")<<std::endl;
        system("ls");
        std::cout<<"No input file specified, please specify one : ";
        std::cin>>file;

    }
    else file=argv[1];
    
    const char* cfile=file.c_str();
    result = ma_decoder_init_file(cfile, NULL, &decoder);
    if (result != MA_SUCCESS)
    {
        printf("Could not load file: %s\n", cfile);
        return -2;
    }
    
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate        = decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &decoder;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        ma_decoder_uninit(&decoder);
        return -3;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return -4;
    }
    ma_uint32 sr=decoder.outputSampleRate;
    int state=0;
    bool run=true;
    ma_uint64 pos,max;
    ma_decoder_get_length_in_pcm_frames(&decoder,&max);
    while(run)
    {
          std::thread thrd(press);
            
          std::cout << " "<<pos/sr<<"s\t";
          std::cout<<"[";
          for(int i=1;i<50;i++)
            {
              if(i<((50*pos)/max))
              {
              std::cout <<'=';
              }
              else
              {
              std::cout << '-';
              }
            }
            std::cout <<"]"<<std::endl;
            std::cout <<"\033[1A";


                if((c=='s'||c=='S')&& ma_device_get_state(&device)==2)
                {
                    //std::cout << "Key pressed: " << c<<std::endl;
                    c='\0';
                    std::cout<<"\033[62CStop playing      \033[1A" << std::endl;
                    ma_device_stop(&device);
                }
                if((c=='c'||c=='C')&& ma_device_get_state(&device)==1)
                {
                  //  std::cout << "Key pressed: " << c<<std::endl;
                    c='\0';
                    std::cout<<"\033[62CContinue Playing\033[1A"<<std::endl;
                    ma_device_start(&device);
                    
                }
                //std::cout << "Key pressed: " << c<<std::endl;
                ma_decoder_get_cursor_in_pcm_frames(&decoder,&pos);
                if(c=='a'&&pos>5000){
                  c='\0';
                  state=ma_device_get_state(&device);
                  
                  ma_device_stop(&device);
                  if(pos>250000){
                  ma_decoder_seek_to_pcm_frame(&decoder,pos-250000);
                  }else {
                  ma_decoder_seek_to_pcm_frame(&decoder,1);
                  }
                  if(state==2)
                {
              
                    ma_device_start(&device);
                }
                 }
                if(c=='d'&&pos<max-250000){
                  c='\0';
                  state=ma_device_get_state(&device);
                  
                  ma_device_stop(&device);
                  ma_decoder_seek_to_pcm_frame(&decoder,pos+250000);
                if(state==2)
                {
              
                    ma_device_start(&device);
                }
            
                }
                
                if(c=='E'||pos==max)
                {
                    printf("EXITING...");
                    
                    run=false;
                }

            usleep(100000);
            thrd.detach();
    }

    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
    t.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    usleep(1000);
    return 0;
}
void press(){
read(STDIN_FILENO, &c, 1);

}
