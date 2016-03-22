function  genPairsbyTrack( base_path )

%base_path= '/home/dragon123/IDT/improved_trajectory_release/saveimgs/bike2';

img_path = [base_path '/imgs/'];
save_path= [base_path '/pairs/'];

if ~isdir(save_path)
    mkdir(save_path);
end

%framegap = 15;
framegap = 30;
framemod = 10;
%leftthres = 20;
leftthres = 40;

txtfile =[base_path '/prunebbox2.txt'];
trackfile = [base_path '/trackbbox2.txt'];
fid = fopen(txtfile, 'r');
if fid < 0 
    return;
end

fidtrack = fopen(trackfile, 'w');
if fidtrack < 0
    fclose(fid);
    return;
end


cnt = 1;

while (~feof(fid))
    
    frameid = fscanf(fid, '%d', 1);
    if size(frameid,1) == 0
        break;
    end
    x1 =  fscanf(fid, '%d', 1);
    if(size(x1, 1) == 0) 
        break;
    end
    y1 =  fscanf(fid, '%d', 1);
    if(size(y1, 1) == 0) 
        break;
    end
    x2 =  fscanf(fid, '%d', 1);
    if(size(x2, 1) == 0) 
        break;
    end
    y2 =  fscanf(fid, '%d', 1);
    if(size(y2, 1) == 0) 
        break;
    end
    x2 = x1 + 50;
    y2 = y1 + 50;
    src = fscanf(fid, '%f', 1);
    if(size(src, 1) == 0) 
        break;
    end
    leftframes = fscanf(fid, '%d', 1);
    if(size(leftframes, 1) == 0) 
        break;
    end
    
    if leftframes > leftthres && mod(leftframes, framemod) == 0
        
        startFrame = frameid;
        endFrame   = frameid + framegap;
        [bbox] = run_tracker_yt(img_path, startFrame, endFrame, x1,y1,x2,y2);
        
        
        if numel(bbox) == 0
            continue;
        end
        
        bx1 = int32(bbox(1)); 
        by1 = int32(bbox(2));
        bx2 = int32(bbox(3));
        by2 = int32(bbox(4));
        
        startName = num2str(startFrame, '%07i.jpg');
        endName = num2str(endFrame, '%07i.jpg');
        startName = [img_path startName]; 
        endName   = [img_path endName]; 
        startim   = imread(startName); 
        endim     = imread(endName); 
        
        h = size(startim, 1);
        w = size(startim, 2);
        
        fprintf(fidtrack, '%d %d %d %d ', startFrame, endFrame, w, h);
        fprintf(fidtrack, '%d %d %d %d ', x1, y1, x2, y2);
        fprintf(fidtrack, '%d %d %d %d\n', bx1, by1, bx2, by2);
        
%         
%         
%         cropim1 = startim(y1:y2, x1:x2, :); 
%         %cropim2 = endim(y1:y2, x1:x2, :);
%         csize = size(cropim1);
%         cropim2 = zeros(csize);
%         
%         bx1 = int32(bbox(1)); 
%         by1 = int32(bbox(2));
%         bx2 = int32(bbox(3));
%         by2 = int32(bbox(4));
%         
%         nbx1 = max([1, bx1]);
%         nby1 = max([1, by1]);
%         nbx2 = min([bx2, w]);
%         nby2 = min([by2, h]);
%         
%         sbx1 = nbx1 - bx1;
%         sby1 = nby1 - by1;
%         sbx2 = bx2 - nbx2;
%         sby2 = by2 - nby2;
%         
%         cropim2(sby1 + 1:csize(1)-sby2, sbx1 + 1:csize(2)-sbx2 , :) = endim(nby1:nby2, nbx1:nbx2,:);
%         
%         name1 = [save_path num2str(cnt) '_1.jpg'];
%         name2 = [save_path num2str(cnt) '_2.jpg'];
%         imwrite(uint8(cropim1), name1);
%         imwrite(uint8(cropim2), name2);
        cnt = cnt + 1;
        
    end
    
    
end


fclose(fid);
fclose(fidtrack);


end

