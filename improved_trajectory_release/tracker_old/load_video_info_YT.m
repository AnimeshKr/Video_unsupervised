function [ img_files, pos, target_sz, video_path ] = load_video_info_YT( base_path, startFrame, endFrame, x1,y1,x2,y2 )

%LOAD_VIDEO_INFO
%   Loads all the relevant information for the video in the given path:
%   the list of image files (cell array of strings), initial position
%   (1x2), target size (1x2), the ground truth information for precision
%   calculations (Nx2, for N frames), and the path where the images are
%   located. The ordering of coordinates and sizes is always [y, x].
%
%   Joao F. Henriques, 2014
%   http://www.isr.uc.pt/~henriques/


	video_path = [base_path '/'];

	%try to load ground truth from text file (Benchmark's format)
%	filename = [video_path 'groundtruth_rect' suffix '.txt'];
%	f = fopen(filename);
%	assert(f ~= -1, ['No initial position or ground truth to load ("' filename '").'])
	
	
    pos = [ floor((y1 + y2) / 2), floor((x1 + x2) / 2)];
    target_sz = [(y2 - y1), (x2 - x1)];
    
    %list specified frames. try png first, then jpg.
    if exist(sprintf('%s%07i.png', video_path, startFrame), 'file'),
        img_files = num2str((startFrame : endFrame)', '%07i.png');

    elseif exist(sprintf('%s%07i.jpg', video_path, startFrame), 'file'),
        img_files = num2str((startFrame : endFrame)', '%07i.jpg');

    else
        error('No image files to load.')
    end

    img_files = cellstr(img_files);
	
end

