
addpath('./tracker/');

% videosrc = '/home/xiaolonw/videos/data/YouTube/general';
videosrc = '/home/xiaolonw/videos/data/YouTube/collect';

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
    if ~isdir(vdir) 
	continue;
    end

    videos = dir([vdir '/*.mp4']);
    if numel(videos) == 0
	continue;
    end
    
    save_vdir= [videosrc '/' sequence '/pairs_traj']; 
    if ~isdir(save_vdir)
	fprintf('%d %s\n', cnt, save_vdir);
        mkdir(save_vdir);
    end
    
    for vi = 1 : numel(videos)    
         cnt = cnt + 1;
%         if cnt > 30 
%             break;
%         end
        vname =  videos(vi).name;
        videoname = [ vdir '/' vname];
        vfname = vname(1:end-4);
        pairdir = [save_vdir '/' vfname];
	save_lock = [save_vdir '/' vfname '_lock_wxl_track'];
        if ~makeDirOrFail(save_lock)
            continue;
        end
        fprintf('%d %s\n', cnt, pairdir);
        
        genPairsbyTrack(pairdir);
        
        
    end
%     
%     if cnt > 30 
%         break;
%     end
end






