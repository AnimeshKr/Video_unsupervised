
addpath('./tracker/');

videosrc = '/home/xiaolonw/videos/data/YouTube/smalltest/';

filesDir = dir(videosrc);
sequences = {};
for i=1:numel(filesDir)
    name = filesDir(i).name;
    if name(1) ~= '.', sequences{end+1} = filesDir(i).name; end
end

cnt = 0;


for t = 1 : numel(sequences)
    
    sequence = sequences{t};
    
    vdir = [videosrc '/' sequence '/videos']; 
    videos = dir([vdir '/*.mp4']);
    
    save_vdir= [videosrc '/' sequence '/pairs']; 
    if ~isdir(save_vdir)
        mkdir(save_vdir);
    end
    
    for vi = 1 : numel(videos)    
%         fprintf('%d\n',vi);
%         cnt = cnt + 1;
%         if cnt > 30 
%             break;
%         end
        vname =  videos(vi).name;
        videoname = [ vdir '/' vname];
        vfname = vname(1:end-4);
        pairdir = [save_vdir '/' vfname];
        if ~isdir(pairdir)
            mkdir(pairdir);
        end
        imgpairdir = [pairdir '/imgs' ];
        if ~isdir(imgpairdir)
            mkdir(imgpairdir);
        end
        
        %tic;
        cmd1 = ['./release/DenseTrackStab ' videoname ' ' pairdir ];
        system(cmd1);
        cmd2 = ['./calbbox ' pairdir];
        system(cmd2);
        cmd3 = ['./release/Video ' videoname ' ' pairdir];
        system(cmd3);
        
        genPairsbyTrack(pairdir);
        
        cmd4 = ['rm -rf ' imgpairdir];
        system(cmd4);
        
        cmd5 = ['mv ' pairdir '/prunebbox2.txt ' pairdir '/pairs2'];
        system(cmd5);
        
        cmd6 = ['rm -rf ' pairdir '/*.txt'];
        system(cmd6);
        %toc;
        
    end
%     
%     if cnt > 30 
%         break;
%     end
end






