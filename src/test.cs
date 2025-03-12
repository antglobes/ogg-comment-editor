using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace AMP.src
{
    class Program
    {
        [DllImport(@"add_ogg_comment.dll")]
        public static extern int amp_AddOggComment([MarshalAs(UnmanagedType.LPStr)]string path, float mindist, float maxdist, float basevol, int sndtype, float maxaidist);

        public static Dictionary<string, string> ReadConfig()
        {
            var assembly = Assembly.GetExecutingAssembly();
            var fileDirectory = Path.GetDirectoryName(assembly.Location);
            var filePath = $"{fileDirectory}\\Config.ini";

            if (File.Exists(filePath) == false)
                throw new Exception($"The Config.ini file is missing at {filePath}.\nPlease run Config.exe to generate it.");
            
            var lines = File.ReadLines(filePath);
            var dict = new Dictionary<string, string>();
            
            foreach (var line in lines)
            {
                var split = line.Split(new[] { '=' }, StringSplitOptions.RemoveEmptyEntries);

                if (split.Length != 2)
                    continue;
                
                if (dict.ContainsKey(split[0].Trim()) == false)
                    dict.Add(split[0].Trim(), split[1].Trim());
            }

            var validationMessage = new StringBuilder();
            if (dict.ContainsKey("GamedataDir") == false)
                validationMessage.AppendLine("The StalkerPath value is empty or missing.");
            if (dict.ContainsKey("FfmpegBinDir") == false)
                validationMessage.AppendLine("The AutoRun value is empty or missing.");
            if (dict.ContainsKey("InputDir") == false)
                validationMessage.AppendLine("The SaveName value is empty or missing.");

            if (validationMessage.Length > 0)
            {
                validationMessage.AppendLine("Please run Config.exe to configure the application.");
                throw new Exception(validationMessage.ToString());
            }

            return dict;
        }

        public static void ValidatePaths(Dictionary<string, string> config)
        {
            foreach (var kvp in config)
            {                
                if (Directory.Exists(kvp.Value) == false)
                    throw new Exception($"The {kvp.Key} {kvp.Value} does not exist.");
            }
        }

        static string[] GetSoundFiles(Dictionary<string, string> config, string suffix){
            var soundDir = $"config["GamedataDir"]\\config["InputDir"]";
            var soundFiles = Directory.GetFiles(soundDir, suffix, SearchOption.AllDirectories);
            return soundFiles;
        }

        static string GetFfmpegExe(Dictionary<string, string> config)
        {
            var exeLocation = $"config["FfmpegBinDir"]\\ffmpeg.exe"
            var exeLocationFileInfo = new FileInfo(exeLocation);
            if (exeLocationFileInfo.Exists)
            {
                return exeLocation;
            }

            throw new Exception($"Could not find \"{exeLocation}\". \n Please Install Ffmpeg or correct the 'FfmpegBinDir' value to ffmpeg's bin folder")
        }

        static async Task ConvertSoundFile(string ffmpegExe, string path, string outpath_wav, string outpath_ogg)
        {
            var FfmpegProcess = new Process();
            FfmpegProcess.StartInfo.Filename = ffmpegExe;
            FfmpegProcess.StartInfo.Arguments = $"-y -f mp3 -i {path} -acodec pcm_s16le -vn -f wav -o {outpath_wav}";
            FfmpegProcess.StartInfo.RedirectStandardOutput = true;
            FfmpegProcess.StartInfo.RedirectStandardError = true;
            FfmpegProcess.start();

            await FfmpegProcess.HasExited;
            
            var FfmpegProcess = new Process();
            FfmpegProcess.StartInfo.Filename = ffmpegExe;
            FfmpegProcess.StartInfo.Arguments = $"-y -f wav -i {outpath} -acodec libvorbis -y -vn -ac 1 -ar 44100 -f ogg -o {outpath_ogg}";
            FfmpegProcess.StartInfo.RedirectStandardOutput = true;
            FfmpegProcess.StartInfo.RedirectStandardError = true;
            FfmpegProcess.start();

            await FfmpegProcess.HasExited;
        }

        static void AddOggComment(string path)
        {
            float mindist = atof("1.0");
            float maxdist = atof("150.0");
            float maxaidist = atof("1.0");
            float basevol = atof("1.0");
            int sndtype = 134217856;
            amp_AddOggComment(path, mindist, maxdist, basevol, sndtype, maxaidist);
        }

        static void MoveFile(Dictionary<string, string> config, string srcFile)
        {
            string outDir = $"config["GamedataDir"]}\\sounds\\output";
            string destFile = @"{outDir}\\{Path.GetFilename(srcFile)}";
            var moved = true;
            while (moved == false)
            {
                try
                {
                    if (!Directory.Exists(outDir))
                    {
                        Directory.CreateDirectory(outDir);
                    }
                    if (!File.Exists(destFile))
                    {
                        File.Move(srcFile, destFile);
                    }
                }
                catch (Exception e)
                {
                    Console.WriteLine("The process failed: {0}", e.ToString());
                }
            }
        }

         public static async Task ProcessSoundFiles(Dictionary<string, string> config, string[] soundFiles)
        {   
            string ffmpegExe = GetFfmpegExe(config);
            var tasks = soundFiles.Select(path => ProcessSoundFile(config, file));
            await Task.WhenAll(tasks);
        }

        static async Task ProcessSoundFile(Dictionary<string, string> config, string file)
        {
            string outpath_wav = $"{config["GamedataDir"]}\\sounds\\output\\{Path.GetFileNameWithoutExtension(path)}.wav";
            string outpath_ogg = $"{Path.GetDirectoryName(outpath)}\\{Path.GetFileNameWithoutExtension(outpath)}.ogg";
        
            await ConvertSoundFile(ffmpegExe, path, outpath_wav, outpath_ogg);
            AddOggComment(outpath_ogg);
            MoveFile(config, outpath_ogg);
        }


        public static async Task Main(string[] args)
        {
            try
            {
                var config = this.ReadConfig();
                this.ValidatePaths(config);
                string [] soundFiles = this.GetSoundFiles(config, "*.mp3");
                await this.ProcessSoundFiles(config, soundFiles);
                Console.Write("Finished Processing MP3 Sound Files");
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                Console.ReadKey();
            }
        }
    }
}