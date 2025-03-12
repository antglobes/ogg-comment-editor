﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;
using System.Globalization;

namespace AMP.src
{
    class Program
    {
        [DllImport(@"add_ogg_comment.dll")]
        public static extern int amp_AddOggComment([MarshalAs(UnmanagedType.LPStr)]string path, float mindist, float maxdist, float basevol, uint sndtype, float maxaidist);

        public static Dictionary<string, string> ReadConfig()
        {
            var assembly = Assembly.GetExecutingAssembly();
            var fileDirectory = Path.GetDirectoryName(assembly.Location);
            var filePath = $"{fileDirectory}\\config.ini";

            if (File.Exists(filePath) == false)
                throw new Exception($"The Config.ini file is missing at {filePath}.");
            
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
                validationMessage.AppendLine("The GamedataDir value is empty or missing.");
            if (dict.ContainsKey("FfmpegBinDir") == false)
                validationMessage.AppendLine("The FfmpegBinDir value is empty or missing.");
            if (dict.ContainsKey("InputDir") == false)
                validationMessage.AppendLine("The InputDir value is empty or missing.");
            if (dict.ContainsKey("GameSndType") == false)
                validationMessage.AppendLine("The GameSndType value is empty or missing.");
            if (dict.ContainsKey("BaseSndVol") == false)
                validationMessage.AppendLine("The BaseSndVol value is empty or missing.");
            if (dict.ContainsKey("MinDist") == false)
                validationMessage.AppendLine("The MinDist value is empty or missing.");
            if (dict.ContainsKey("MaxDist") == false)
                validationMessage.AppendLine("The MaxDist value is empty or missing.");
            if (dict.ContainsKey("MaxAIDist") == false)
            validationMessage.AppendLine("The MaxAIDist value is empty or missing.");



            if (validationMessage.Length > 0)
            {
                validationMessage.AppendLine("Please run Config.exe to configure the application.");
                throw new Exception(validationMessage.ToString());
            }

            return dict;
        }

        public static void ValidatePaths(Dictionary<string, string> config)
        {
            Console.Write("Validating Paths \n");
            foreach (var kvp in config)
            {                
                
                if (Regex.IsMatch(kvp.Value, @"\w+:\\") && Directory.Exists(kvp.Value) == false)
                    throw new Exception($"The {kvp.Key} {kvp.Value} does not exist.\n");
            }
        }

        static string[] GetSoundFiles(Dictionary<string, string> config, string suffix)
        {
            var soundDir = $"{config["InputDir"]}";
            Console.Write($"Getting sound files from {soundDir} \n");
            var soundFiles = Directory.GetFiles(soundDir, suffix, SearchOption.AllDirectories);
            var playlistCount = Directory.GetDirectories(soundDir).Count(s =>
            {
                Console.Write($"Directory: {s} \n");
                return Regex.IsMatch(s, "_playlist_\\d+");
            });
            Console.Write($"Playlist Count: {playlistCount} \n");
            config.Add("PlaylistCount", playlistCount.ToString());
            return soundFiles;
        }

        static string GetFfmpegExe(Dictionary<string, string> config)
        {
            var exeLocation = $"{config["FfmpegBinDir"]}\\ffmpeg.exe";
            Console.Write($"Searching for ffmpeg exe at {exeLocation} \n");
            var exeLocationFileInfo = new FileInfo(exeLocation);
            if (exeLocationFileInfo.Exists)
            {
                Console.Write($"Found ffmpeg exe \n");
                return exeLocation;
            }

            throw new Exception($"Could not find {exeLocation}. \n Please Install Ffmpeg or correct the 'FfmpegBinDir' value to ffmpeg's bin folder\n");
        }

        static async Task ConvertSoundFile(string ffmpegExe, string i_args, string o_args, string path, string outpath)
        {
            Console.Write($"Converting {path} to {outpath}\n");

            var directory = Path.GetDirectoryName(outpath);
            if (!Directory.Exists(directory))
            {
                Console.Write($"Output Directory: {directory} doesn't exist\n");
                Directory.CreateDirectory(directory);
                Console.Write($"Created Directory: {directory}\n");
            }

            Process FfmpegProcess = new Process();
            FfmpegProcess.StartInfo.FileName = ffmpegExe;
            FfmpegProcess.StartInfo.Arguments = $"{i_args} -i \"{path}\" {o_args} \"{outpath}\"";
            FfmpegProcess.StartInfo.RedirectStandardError = true;
            FfmpegProcess.StartInfo.RedirectStandardOutput = true;
            FfmpegProcess.StartInfo.UseShellExecute = false;
            FfmpegProcess.StartInfo.CreateNoWindow = false;
            FfmpegProcess.EnableRaisingEvents = true;
            FfmpegProcess.Start();

            await Task.Run(() => {
                Console.WriteLine(FfmpegProcess.StandardError.ReadToEnd());
            });

            await FfmpegProcess.WaitForExitAsync();
            Console.Write($"Finished Converting {path} \n");
        }

        static float stringToFloat(string str)
        {
            return float.Parse(str, CultureInfo.InvariantCulture.NumberFormat);
        }

        static uint gameSndTypeToInt(string sndType) 
        {
            var sndTypeTbl = new Dictionary<string, uint>();

            sndTypeTbl.Add("world_ambient", 134217856);
            sndTypeTbl.Add("object_exploding", 134217984);
            sndTypeTbl.Add("object_colliding", 134218240);
            sndTypeTbl.Add("object_breaking", 134218752);
            sndTypeTbl.Add("anomaly_idle", 268437504);
            sndTypeTbl.Add("npc_eating", 536875008);
            sndTypeTbl.Add("npc_attacking", 536879104);
            sndTypeTbl.Add("npc_talking", 536887296);
            sndTypeTbl.Add("npc_step", 536903680);
            sndTypeTbl.Add("npc_injuring", 536936448);
            sndTypeTbl.Add("npc_dying", 537001984);
            sndTypeTbl.Add("item_using", 1077936128);
            sndTypeTbl.Add("item_taking", 1082130432);
            sndTypeTbl.Add("item_hiding", 1090519040);
            sndTypeTbl.Add("item_dropping", 1107296256);
            sndTypeTbl.Add("item_picking_up", 1140850688);
            sndTypeTbl.Add("weapon_recharging", 2147745792);
            sndTypeTbl.Add("weapon_bullet_hit", 2148007936);
            sndTypeTbl.Add("weapon_empty_clicking", 2148532224);
            sndTypeTbl.Add("weapon_shooting", 2149580800);
            sndTypeTbl.Add("undefined", 0);

            return sndTypeTbl[sndType];
        }

        static void AddOggComment(Dictionary<string, string> config, string path)
        {
            if (!File.Exists(path))
            {
                throw new Exception($"Cannot add ogg comment, file not found at {path}");
            }
            
            float basevol = stringToFloat(config["BaseSndVol"]);
            float mindist = stringToFloat(config["MinDist"]);
            float maxdist = stringToFloat(config["MaxDist"]);
            float maxaidist = stringToFloat(config["MaxAIDist"]);
            
            uint sndtype = gameSndTypeToInt(config["GameSndType"]);
            Console.Write($"Adding Ogg Comment: MinDist:{mindist}, MaxDist:{maxdist}, BaseVol:{basevol}, SndType:{sndtype}, MaxAIDist:{maxaidist} \n");
            amp_AddOggComment(path, mindist, maxdist, basevol, sndtype, maxaidist);
            Console.Write($"\nOgg Comment Added to {path}\n");
        }

        static void MoveFile(Dictionary<string, string> config, string srcFile)
        {
            string outDir = $"{config["GamedataDir"]}\\sounds\\trx\\radio";
            string destFile = $"{outDir}\\{Path.GetFileName(srcFile)}";
            Console.Write($"Moving {srcFile} to {destFile}\n");
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
                    Console.WriteLine($"The process failed: {0}\n", e.ToString());
                }
            }
        }

        static void RemoveFile(string srcFile)
        {
            if (!File.Exists(srcFile))
            {
                throw new Exception($"Cannot delete {srcFile} asi it doesn't exist.\n");
            }
            File.Delete(srcFile);
        }

         public static async Task ProcessSoundFiles(Dictionary<string, string> config, string[] soundFiles)
        {   
            var tasks = soundFiles.Select(path => ProcessSoundFile(config, path));
            await Task.WhenAll(tasks);
        }

        static void CreateRadioLtx(string gamedataDir)
        {
            var outSoundDir = $"{gamedataDir}\\sounds\\trx\\radio";
            var existingPlaylistCount = Directory.GetDirectories(outSoundDir).Count(s =>
            {
                return Regex.IsMatch(s, "_playlist_\\d+");
            });
            
            if (existingPlaylistCount > 0){
                string radioLtxName = "mod_radio_zone_fm_amp.ltx";
                string radioLtxDir = $"{gamedataDir}\\configs\\plugins";
                string radioLtxPath = $"{radioLtxDir}\\{radioLtxName}";
            

                if (!Directory.Exists(radioLtxDir)) 
                {
                    Directory.CreateDirectory(radioLtxDir);
                }
                
                var ltxCode = $"![trx_radio_plyr]\nnumber_of_playlists = {existingPlaylistCount + 2}";

                using Stream configFile = File.Open(radioLtxPath, FileMode.Create);

                Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);
                var encoding = Encoding.GetEncoding(1252);
                var ltxBuffer = encoding.GetBytes(ltxCode);
                configFile.Write(ltxBuffer, 0, ltxBuffer.Length);

                Console.Write($"Created radio playlist ltx at: {radioLtxPath}\n");
            }
        }

        static async Task ProcessSoundFile(Dictionary<string, string> config, string path)
        {
            string ffmpegExe = GetFfmpegExe(config);
            string outpath_dir = Path.GetRelativePath($"{config["InputDir"]}\\", Path.GetDirectoryName(path));
            string outpath_wav = $"{config["GamedataDir"]}\\sounds\\trx\\radio\\{outpath_dir}\\{Path.GetFileNameWithoutExtension(path)}.wav";
            string outpath_ogg = $"{Path.GetDirectoryName(outpath_wav)}\\{Path.GetFileNameWithoutExtension(path)}.ogg";

            if (File.Exists(outpath_ogg))
            {
                Console.Write($"Ogg File Already Exists at: {outpath_ogg}\n");
                return;
            }

            await ConvertSoundFile(ffmpegExe, "-y -f mp3", "-acodec pcm_s16le -vn -f wav", path, outpath_wav);
            await ConvertSoundFile(ffmpegExe, "-y -f wav", "-acodec libvorbis -y -vn -ac 1 -ar 44100 -f ogg", outpath_wav, outpath_ogg);
            AddOggComment(config, outpath_ogg);
            RemoveFile(outpath_wav);
        }
        

        public static async Task Main(string[] args)
        {
            try
            {
                var config = ReadConfig();
                ValidatePaths(config);
                string [] soundFiles = GetSoundFiles(config, "*.mp3");
                await ProcessSoundFiles(config, soundFiles);
                CreateRadioLtx(config["GamedataDir"]);
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