<?php
/************************************************************************/
/* Oscailt                                                              */
/* Indepenent Media Centre Content Management System                    */
/* ==================================================================== */
/* Copyright (c)2003-2005 by Independent Media Centre Ireland           */
/* http://www.indymedia.ie                                              */
/* Development List: oscailt@lists.indymedia.org                        */
/* See contributions.txt for the list of contributors                   */
/*                                                                      */
/* This program is free software. You can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation. http://www.gnu.org/copyleft/gpl.html   */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/************************************************************************/

class ImageTool
{
   // takes in local file or url and transforms it according to image config
   // ouput file is place in imagecache subfolder of attachment store
   // subfolders are as follows
   //  /imagecache/remote for url retrieved files
   //  /imagecache/local/misc for local files not in attachment store
   //  /imagecache/local/<att store subfolders> for for local files in attachment store
   // if cached file already exists transformation is not performed
   // returns an array of info about the url
   // 0 => url of scaled down image (or original depending on settings)
   // 1 => url of full-size image (for linking to it)
   // 2 => length and width string for inserting in an img tag
   var $show_error = false;
   var $is_error = false;
   var $error_text = "";
   var $error_image;

   function ImageTool()
   {
      global $system_config;
      $this->error_image = $system_config->error_image;
   }

   function getTransformedImageURL($inputFile,$imageConfig, $generateThumbnail=true)
   {
      global $system_config, $performance_test;
      $url_info = array();
      if($performance_test > 9) markTime("file being transformed $inputFile");
      $urlinfo=parse_url($inputFile);
      //determine if file is local or url, and generate transformed file name
      if(isset($urlinfo["scheme"]))
      {
         $is_remote = true;
         //remove chars not allowable in file names from url
         $transformedFileName="remote/".$imageConfig->getFilePrefix().ereg_replace("[^a-z0-9._]", "", str_replace(" ", "_", str_replace("%20", "_", $inputFile)));
         $iFile = $inputFile;
      }
      else
      {
         $iFile = realpath($inputFile);
         $is_remote = false;
         if($iFile == "")
         {
            $iFile = $inputFile;
         }
         $attach_store_full = realpath($system_config->attachment_store);
         $transformedFileName=$imageConfig->getFilePrefix().basename($iFile);
         //determine if local file is in attachment folder or elsewhere
         $pos=strpos($iFile,$attach_store_full);
         if($pos === false || $pos>0)
         {
            // file is from elsewhere so place in misc directory
            $transformedFileName="local/misc/".$transformedFileName;
         }
         else
         {
            // file is from attachment store so recreate folder stucture under local
            $f = str_replace($attach_store_full,"",dirname($inputFile));
            $transformedFileName="local/".$f."/".$transformedFileName;
         }
      }
      $transformedFileName=$system_config->image_cache."/".$transformedFileName;

      // replace input extension with jpg extension as all output is jpg
      $pathinfo=pathinfo($transformedFileName);
      $transformedFileName=$pathinfo["dirname"]."/".str_replace(".".$pathinfo["extension"],"",$pathinfo["basename"]).".jpg";
      //if transformed file doesn't already exist then create it
      // echo "check transformed ".$transformedFileName."<BR>";
      if(!file_exists($transformedFileName))
      {
      //echo " does not exist <BR>";
         $size = $this->getImageSize($iFile, $is_remote);
         //check to see if we don't need to resize
         if($size != 0 and $size[0] > 0 and $size[1] > 0 and $size[2] > 0 and ($imageConfig->requiresResize($size[0], $size[1], $size[2]) === false))
         {
            //then check to see if we are supposed to be linking to a remote server
            if(!$is_remote)
            {
               //we have a local copy of the file...
               $file_path = str_replace($system_config->attachment_store, $system_config->attachment_store_url, $inputFile);
               //is it a local link
               if(!isLocalPath($file_path))
               {
                  $orig_size = $this->getImageSize($file_path, true);
                  if($orig_size == 0 or $orig_size[0] <= 0 or $orig_size[1] <= 0 or $orig_size[2] <= 0)
                  {
                     if(isAbsolutePath($inputFile))
                     {
                        $file_path = getAbsolutePathAsRelative($inputFile);
                     }
                     else
                     {
                        $file_path = $inputFile;
                     }
                  }
               }
               $url_info[0] = $file_path;
               $url_info[1] = $file_path;
               $url_info[2] = "width='".$size[0]."' height='".$size[1]."'";
               //return $file_path;
      //echo " Return Info <BR>";
               return $url_info;
            }
            else
            {
               $url_info[0] = $inputFile;
               $url_info[1] = $inputFile;
               $url_info[2] = "width='".$size[0]."' height='".$size[1]."'";
      //echo " Return Info 2<BR>";
               return $url_info;
            }
         }

         // create cache folder structure
         $splitdirs=@explode("/",dirname($transformedFileName));
         $dirname="";
         for($i=0;$i<count($splitdirs);$i++)
         {
            $dirname=$dirname.$splitdirs[$i]."/";
            if(!file_exists($dirname))
            {
               if (!mkdir($dirname,$system_config->default_writable_directory_permissions)) {
	           logMessage("Failure to create directory ".$dirname. " with permissions ".$system_config->default_writable_directory_permissions,"sitelog.txt");
               }
            }

         }

	 //echo "Gen ...<BR>";
         if ( $generateThumbnail == true) {
		 //echo "generating......<BR>";
             if($this->transformImage($iFile,$transformedFileName,$imageConfig)==false) return null;
             if($performance_test > 10) markTime("file transformed $transformedFileName");
	 } 
      }
      else
      {
         if($performance_test > 9) markTime("file exists $transformedFileName");
      }
      $url_info[0] = $transformedFileName;
      if(strpos($inputFile, $system_config->attachment_store_url) === false and !isAbsoluteURL($inputFile))
      {	
	if ($t_newfile != "") {	
	    $image_filename = $t_newfile;
	    $t_attachment->attachment_file = str_replace($system_config->attachment_store,"",$image_filename);
         }
         $orig_url = str_replace($system_config->attachment_store, $system_config->attachment_store_url, $inputFile);
      }
      else
      {
         $orig_url = $inputFile;
      }
      $url_info[1] = $orig_url;
      //check to see if we don't need to resize
      $url_info[2] = "";
      if($system_config->width_height_on_all_image_tags)
      {
         $size = $this->getImageSize($transformedFileName, false);
         if($size != 0 and $size[0] > 0 and $size[1] > 0 and $size[2] > 0 and ($imageConfig->requiresResize($size[0], $size[1], $size[2]) === false))
         {
            $url_info[2] = "width='$size[0]' height='$size[1]'";
	    $url_info[3] = $size[0];
            $url_info[4] = $size[1];
         }
      }
      if($performance_test > 9) markTime("file transform finished");
      // echo " Return url_info <BR>";
      return $url_info;
   }

   function getImageSize($url, $is_remote)
   {
      global $performance_test, $system_config;
      if($is_remote)
      {
         //try to get the image details from cache
         $size = $this->getCachedImageSizeDetails($url);
         if($size !== false)
         {
            if(!isset($size[0]) or !isset($size[1]) or !isset($size[2]) or $size[0] <= 0 or $size[1] <= 0 or $size[2] <= 0)
            {
               if(time() < $size[3] + $system_config->remote_image_query_caching_healing_timeout)
               {
                  return $size;
               }
            }
            else
            {
               return $size;
            }
         }
      }
      $size = @getimagesize($url);
      if($size)
      {
         if(!$is_remote && file_exists($url))
         {
            $size[2] = filesize($url);
         }
         elseif($is_remote)
         {
            if($performance_test > 5) markTime("Fetched Remote Size Info for $url");
            $size[2] = remote_file_size ($url);
            $this->cacheImageSizeDetails($url, $size);
         }
         else return false;
      }
      //else it doesn't exist... maybe put in a timeout...
      else
      {
         $size = array(0, 0, 0, time());
         $this->cacheImageSizeDetails($url, $size);
      }
      return $size;
   }

   function getCachedImageSizeDetails($url)
   {
      global $system_config;
      $fname = $system_config->private_cache."remoteimagesizecache/".$this->getDetailsCacheFileNameFromURL($url);
      if($fname !== false && file_exists($fname))
      {
         $size = unserialize(implode('',file($fname)));
         if($size && is_array($size)) return $size;
      }
      return false;
   }

   function cacheImageSizeDetails($url, $size)
   {
      global $system_config;
      $dirname = $system_config->private_cache."remoteimagesizecache/";
      $fname = $dirname.$this->getDetailsCacheFileNameFromURL($url);
      $splitdirs=@explode("/",dirname($fname));
      $dirname="";
      for($i=0;$i<count($splitdirs);$i++)
      {
         $dirname=$dirname.$splitdirs[$i]."/";
         //if(!file_exists($system_config->attachment_store.$dirname)) mkdir($system_config->attachment_store.$dirname,$system_config->default_writable_directory_permissions);
         if(!file_exists($dirname))
         {
           // echo "creating ".$dirname."<br />";
            if (!mkdir($dirname,$system_config->default_writable_directory_permissions)) {
	       logMessage("Failure to create directory ".$dirname. " with permissions ".$system_config->default_writable_directory_permissions. " in function cacheImageSizeDetails","sitelog.txt");
            }
         }
      }
      $fp = fopen($fname,"w");
      if (flock($fp, LOCK_EX))
      {
         fputs($fp, serialize($size));
         flock($fp, LOCK_UN);
         fclose($fp);
      }
      else
      {
         fclose($fp);
         return false;
      }
      return true;
   }

   function getDetailsCacheFileNameFromURL($url)
   {
      $cache_path = array();
      $urlinfo=parse_url($url);
      //determine if file is local or url, and generate transformed file name
      if(isset($urlinfo["scheme"]) && isset($urlinfo['host']))
      {
         $cache_path[] = $urlinfo['host'];
      }
      if(isset($urlinfo['path']))
      {
         $pathinfo = pathinfo($urlinfo['path']);
         $cache_path[] = $pathinfo['dirname'];
         $basename = $pathinfo['basename'];
         $extension = $pathinfo['extension'];
         if($extension)
         {
            $basename = str_replace(".".$extension, "", $basename);
         }
         if($basename)
         {
            $basename .= ".cache";
            $cache_path[] = $basename;
         }
         else return false;

      }
      $x = implode("/", $cache_path);
      if($x == "") return false;
      else return $x;
  }

   function getErrorImage($file, $img_cfg)
   {
      $im = @imagecreatefrompng($this->error_image);
      $error = $img_cfg->error_text;
      if($im == "")
      {
         $w = ($img_cfg && $img_cfg->maxWidth > 0) ?  $img_cfg->maxWidth : 100;
         $h = ($img_cfg && $img_cfg->maxHeight > 0) ?  $img_cfg->maxHeight : 75;
         $im=imagecreate($w, $h);
         $tc=imagecolorallocate($im, 0, 0, 0);
         $bgc=imagecolorallocate($im, 255, 255, 255);
         imagefilledrectangle($im, 0, 0, 100, 75, $bgc);
         $tc=imagecolorallocate($im, 0, 0, 0);
         imagestring($im, 3, 2, 2, $error, $tc);
         imagestring($im, 1, 2, 20, basename($file), $tc);
      }
      else
      {
         $tc=imagecolorallocate($im, 23, 23, 127);
         //imagestring($im, 5, 2, 2, $error, $tc);
         imagestring($im, 2, 1, 2, basename($file), $tc);

	 // If it fails try again with a smaller font.
         if (imagestring($im, 2, 1,15, trim($this->error_text), $tc) == FALSE)
             imagestring($im, 1, 1,15, trim($this->error_text), $tc);
      }
      return $im;
   }

   // takes in local file or url and transforms it according to image config
   // returns true if image created, false if error occurred
   // all output is jpg so ensure output file has correct extension
   function transformImage($inputFile,$outputFile,$imageConfig)
   {
        global $system_config;
        $originalImage = "";
        $pathinfo=pathinfo($inputFile);
        $inputType=strtolower($pathinfo["extension"]);
        if(!isLocalPath($inputFile) && (strtolower(ini_get('allow_url_fopen')) === 'off' or ini_get('allow_url_fopen') == 0))
        {
            $local_copy = $this->createLocalCopyOfImage($inputFile);
            if($local_copy)
            {
               // Add checks like below to check the image memory requirements.
               if($this->enoughMemoryForImage($local_copy) == true) {
                   if($inputType=="jpg" or $inputType=='jpeg') $originalImage = @imagecreatefromjpeg($local_copy);
                   else if($inputType=="gif") $originalImage = @imagecreatefromgif($local_copy);
                   else if($inputType=="png") $originalImage = @imagecreatefrompng($local_copy);
                   unlink($local_copy);
	       } else {
                   $originalImage = FALSE;
	       }
            }
            else
            {
               $this->error_text .= "Can not fetch Remote Image without allow_url_open";
            }
            if($originalImage=== FALSE || $originalImage=="")
            {
               if($inputType=="bmp")
               {
                  $this->error_text .= "bmp files are too crap a format to resize (don't use bmps on the web)";
               }
               $this->is_error = true;
               if($this->show_error)
               {
                  $originalImage=$this->getErrorImage($inputFile, $imageConfig);
               }
               else
               {
                  return false;
               }
            }
        } else {
            if(!file_exists($inputFile)) {
	        if ($system_config->user_error_reporting > 1 ) logMessage("File ".$inputFile." for thumbnail generation does not exist.","sitelog.txt");
                $this->is_error = true;
                return false;
	    }
            if($this->enoughMemoryForImage($inputFile) == true) {
                if($inputType=="jpg" or $inputType=='jpeg' or $inputType=="jpe") $originalImage = imagecreatefromjpeg($inputFile);
                else if($inputType=="gif") $originalImage = @imagecreatefromgif($inputFile);
                else if($inputType=="png") $originalImage = @imagecreatefrompng($inputFile);
		else $originalImage = FALSE;
	    } else {
                $originalImage = FALSE;
	    }
        }

      // if load failed
      if($originalImage === FALSE || $originalImage=="")
      {
	 global $system_config;

         $this->is_error = true;
	 // If an error then probably wrong format.
	 if($this->show_error == false) {
             $t_sizeinfo = getimagesize($inputFile);
	     if ($t_sizeinfo[2] == 1 && $inputType != "gif") $this->show_error = true;
	     else if ($t_sizeinfo[2] == 2 && ($inputType != "jpg" || $inputType != "jpeg" || $inputType != "jpe")) $this->show_error = true;
	     else if ($t_sizeinfo[2] == 3 && $inputType != "png") $this->show_error = true;
	     else if ($t_sizeinfo[2] == 6 && $inputType != "bmp") $this->show_error = true;

	     if($this->show_error == true) $this->error_text = "Image not ".$inputType;
	 }

	 if ($system_config->user_error_reporting > 1 ) 
             logMessage("Could not generate thumbnail for image: ".$inputFile." - ".$this->error_text,"sitelog.txt");

         $this->is_error = true;
         if($this->show_error)
         {
            $originalImage = $this->getErrorImage($inputFile, $imageConfig);
         }
         else
         {
            return false;
         }
      }
      if($this->is_error && ($imageConfig->error_height > 0 and $imageConfig->error_width > 0))
      {
         $imageConfig->maxWidth = $imageConfig->error_width;
         $imageConfig->maxHeight = $imageConfig->error_height;
      }
      $originalImageWidth = imagesx($originalImage);
      $originalImageHeight = imagesy($originalImage);


      // crop image instead of resize
      if($imageConfig->cropX1 > 0 || $imageConfig->cropY1 > 0 || $imageConfig->cropX2 > 0 || $imageConfig->cropY2 > 0)
      {
         $croppedImageWidth = $imageConfig->cropX2-$imageConfig->cropX1;
         $croppedImageHeight = $imageConfig->cropY2-$imageConfig->cropY1;
         $croppedImage = imagecreatetruecolor($croppedImageWidth, $croppedImageHeight);
         imagecopy ($croppedImage, $originalImage, 0, 0, $imageConfig->cropX1, $imageConfig->cropY1, $croppedImageWidth, $croppedImageHeight);
         imagedestroy($originalImage);
      }
      else
      {
         $croppedImageWidth = $originalImageWidth;
         $croppedImageHeight = $originalImageHeight;
         $croppedImage = $originalImage;
      }

      // rotate
      if($imageConfig->rotateDegrees > 0)
      {
         $bgcolour = imagecolorallocate($croppedImage, $imageConfig->backgroundColour[0], $imageConfig->backgroundColour[1], $imageConfig->backgroundColour[2]);
         $rotatedImage = imagerotate($croppedImage, $imageConfig->rotateDegrees,$bgcolour);
         $rotatedImageWidth = imagesx($rotatedImage);
         $rotatedImageHeight = imagesy($rotatedImage);
      }
      else
      {
         $rotatedImageWidth = $croppedImageWidth;
         $rotatedImageHeight = $croppedImageHeight;
         $rotatedImage = $croppedImage;
      }

      // calculate scaling
      if($imageConfig->maxWidth > 0 && ($rotatedImageWidth > $imageConfig->maxWidth || $imageConfig->forceToMaxWidth)) $scaledImageWidth = $imageConfig->maxWidth;
      else $scaledImageWidth = $rotatedImageWidth;

      $scaleFactor = $scaledImageWidth/$rotatedImageWidth;
      $scaledImageHeight = round($rotatedImageHeight*$scaleFactor);

      // Oscailt objects can specify just width and leave height zero. That way the scaling should
      // keep the aspect ratio
      // echo "scaleHeight=".$scaledImageHeight . " ww=".$imageConfig->maxWidth. " hh=".$imageConfig->maxHeight ;
      // $imageConfig->forceToMaxHeight =true;
      // $imageConfig->forceToMaxWidth  =true;
      if($imageConfig->maxHeight > 0 && $scaledImageHeight > $imageConfig->maxHeight)
      {
	 //   echo "choce 1<br>";
         // if height now exceeds max lower it and rejig width if possibble
         $scaledImageHeight = $imageConfig->maxHeight;
         if($imageConfig->maxWidth <= 0 || ($imageConfig->maxWidth > 0 && $imageConfig->forceToMaxWidth == false))
         {
	    //  echo "choce 1b<br>";
            $scaleFactor = $scaledImageHeight/$rotatedImageHeight;
            $scaledImageWidth = round($rotatedImageWidth*$scaleFactor);
         } else {
	    //  echo "choce 1c<br>";
            // Perserve aspect ratio but don't bother if only out 10%.
            if ( $systemconfig->preserve_aspect_ratio == true ) {
              if ( ($originalImageWidth/$originalImageHeight) > (1.1 * ($imageConfig->maxWidth/$imageConfig->maxHeight))) {
                  //$imageConfig->maxWidth = floor($imageConfig->maxHeight * ($originalImageWidth/$originalImageHeight));
                  $scaledImageWidth = floor($imageConfig->maxHeight * ($originalImageWidth/$originalImageHeight));
              }
              if ( ($originalImageWidth/$originalImageHeight) < (0.9 * ($imageConfig->maxWidth/$imageConfig->maxHeight))) {
                  //$imageConfig->maxHeight = floor($imageConfig->maxWidth * ($originalImageHeight/$originalImageWidth));
                  $scaledImageHeight = floor($imageConfig->maxWidth * ($originalImageHeight/$originalImageWidth));
              }
            }
         }
      }
      else if($imageConfig->maxHeight > 0 && $imageConfig->forceToMaxHeight)
      {
	 //   echo "choce 2<br>";
         //if height must be set to max do it and rejig width if possibble
         $scaledImageHeight = $imageConfig->maxHeight;
         if($imageConfig->maxWidth <= 0)
         {
	    //  echo "choce 2b<br>";
            $scaleFactor = $scaledImageHeight/$rotatedImageHeight;
            $scaledImageWidth = round($rotatedImageWidth*$scaleFactor);
         } else {
            if ( $systemconfig->preserve_aspect_ratio == true ) {
              if ( ($originalImageWidth/$originalImageHeight) > (1.1 * ($imageConfig->maxWidth/$imageConfig->maxHeight))) {
                  $scaledImageWidth = floor($imageConfig->maxHeight * ($originalImageWidth/$originalImageHeight));
              }
              if ( ($originalImageWidth/$originalImageHeight) < (0.9 * ($imageConfig->maxWidth/$imageConfig->maxHeight))) {
                  $scaledImageHeight = floor($imageConfig->maxWidth * ($originalImageHeight/$originalImageWidth));
              }
            }
         }
      } 

      // perform scaling
      // echo "<BR>scaleW=".$scaledImageWidth." rotWidth=". $rotatedImageWidth." scaleH=".$scaledImageHeight." rotH=". $rotatedImageHeight;
      if($scaledImageWidth != $rotatedImageWidth || $scaledImageHeight != $rotatedImageHeight)
      {
         $scaledImage = imagecreatetruecolor($scaledImageWidth, $scaledImageHeight);
         imagecopyresampled($scaledImage, $rotatedImage, 0, 0, 0, 0, $scaledImageWidth, $scaledImageHeight, $rotatedImageWidth, $rotatedImageHeight);
         imagedestroy($rotatedImage);
      }
      else
      {
         $scaledImage = $rotatedImage;
      }
      touch($outputFile);
      // output file, lowering jpeg quality until below filesize threshold from image config
      for($quality=100;$quality>=10;$quality=$quality-10)
      {
         If(!imagejpeg($scaledImage,$outputFile,$quality))
	 {
	     imagedestroy($scaledImage);
	     return false;
         }
         clearstatcache();
         $filesize=filesize($outputFile);
         if($filesize<=$imageConfig->maxFileSize*1024) break;
      }
      imagedestroy($scaledImage);

      //Read and write for owner(webserver), read for everybody else
      chmod ($outputFile, 0644);
      return true;
   }

   function clearImageCache($clr_main=true,$clr_video=false)
   {
      global $system_config;

      $files_deleted = 0;
      //if(file_exists($system_config->attachment_store."imagecache"))
      if($clr_main == true)
      {
          if(file_exists($system_config->image_cache))
          {
             $files_deleted = $this->performClearImageCache(null);
             rmdir($system_config->image_cache);
    	     reportInfo("Image cache at ".$system_config->image_cache." cleared. No of files deleted was: ".$files_deleted);
          }
          if(file_exists($system_config->private_cache."remoteimagesizecache/"))
          {
             $files_deleted = $this->performClearImageCache($system_config->private_cache."remoteimagesizecache/");
             rmdir($system_config->private_cache."remoteimagesizecache/");
    
    	     reportInfo("Image cache at ".$system_config->private_cache."remoteimagesizecache/ cleared. No of files deleted was: ".$files_deleted);
    
          }
      }
      if($clr_video == true)
      {
    	  $t_cache = $system_config->image_cache."/video_images/";
          if(file_exists($t_cache))
          {
             $files_deleted = $this->performClearImageCache($t_cache);
    	     reportInfo("Image cache at ".$t_cache." cleared. No of files deleted was: ".$files_deleted);
          }
      }
      if ($files_deleted == 0) reportInfo("No files deleted for image cache clear.");
   }

   // Return the number of files deleted for info purposes.
   function performClearImageCache($directory)
   {
      global $system_config;

      if($directory==null) $directory=$system_config->image_cache;
      $dh=opendir($directory) or reportError("Couldn't open image cache directory: ".$directory);

      $number_deleted = 0;
      while($file=readdir($dh))
      {
         if(is_dir($directory."/".$file))
         {
            if($file=="." || $file=="..") continue;

            if ($file != "video_images" ) {
            	$number_deleted = $number_deleted + $this->performClearImageCache($directory."/".$file);
            	rmdir($directory."/".$file);
	    }
         }
         else
         {
            unlink($directory."/".$file);
            $number_deleted++;
         }
      }
      closedir($dh);
      # Why was this here??
      # $this->sql_cache_invalid=false;
      return $number_deleted;
   }

   function createLocalCopyOfImage($inputFile)
   {
       //stub to hold curl file copy and socket file copy...
       return false;
   }

   function enoughMemoryForImage($filename)
   {
       	$max_memory = trim(preg_replace("/[A-Za-z]/","",ini_get('memory_limit')));
	// Windows or early PHP version may return empty so default to 8Mb
       	if (trim(ini_get('memory_limit')) == "") $max_memory = 8;

	$req_memory = $this->getMemoryForImage($filename);
	if ( $req_memory > $max_memory ) {
	     global $system_config;
	     $this->show_error = true;
	     $this->error_text = "Image too big. ".$req_memory." Mb";
	     if ($system_config->user_error_reporting > 1 ) logMessage("Not enough memory to generate thumbnail for image: ".$filename.". Limit is ".$max_memory." Mb requires ".$req_memory." Mb","sitelog.txt");
             return false;
        }
        return true;
  
   }
   function getMemoryForImage($filename, $in_mb=true )
   {
       $imageInfo = getimagesize($filename);
       $MB = 1048576;  // number of bytes in 1M
       $K64 = 65536;    // number of bytes in 64K
       $TWEAKFACTOR = 1.0;  // Or whatever works for you -was 1.5

       if (!isset($imageInfo['channels']) ) $t_channels = 8;
       else $t_channels = $imageInfo['channels'] ;

       $memoryNeeded = round( ( $imageInfo[0] * $imageInfo[1]
                                              * $imageInfo['bits']
                                              * $t_channels / 8
                                              + $K64) * $TWEAKFACTOR);
       //Should call get_memory_usage to see how much is already used.
       //ini_get('memory_limit') only works if compiled with "--enable-memory-limit" also
       //Default memory limit is 8MB so well stick with that.
       //To find out what yours is, view your php.ini file.
       // $memoryLimit = 8 * $MB;
       if ($in_mb == true) $memoryNeeded = round($memoryNeeded/$MB);

       return $memoryNeeded;
   }
}

class ImageConfig
{
   // combinations of max and force values can allow you have width and height scaled in proportion the other
   var $maxWidth;// max width in pixels, 0 if no max
   var $maxHeight;// max height in pixels, 0 if no max
   var $forceToMaxWidth;// if original width is smaller than max shold we enlarge it
   var $forceToMaxHeight;// if original height is smaller than max should we enlarge it

   var $maxFileSize;//the maximum size in bytes of output jpeg, the optimum jpeg compression will be used to attempt to achieve this
   var $backgroundColour;// if rotation is used what colour will background be.
   var $cropX1;
   var $cropY1;
   var $cropX2;
   var $cropY2;
   var $rotateDegrees;// degrees of ratations

   var $error_height = 0;//allow explicit setting of error images
   var $error_width = 0;
   var $error_text = "Invalid Image";

   function ImageConfig($maxWidth,$maxHeight,$forceToMaxWidth,$forceToMaxHeight,$maxFileSize,$backgroundColour,$rotateDegrees,$cropX1,$cropY1,$cropX2,$cropY2)
   {
      $this->maxWidth=$maxWidth;
      $this->maxHeight=$maxHeight;
      $this->forceToMaxWidth=$forceToMaxWidth;
      $this->forceToMaxHeight=$forceToMaxHeight;
      $this->maxFileSize=$maxFileSize;
      $this->backgroundColour=$backgroundColour;
      $this->rotateDegrees=$rotateDegrees;
      $this->cropX1=$cropX1;
      $this->cropY1=$cropY1;
      $this->cropX2=$cropX2;
      $this->cropY2=$cropY2;
   }

   function requiresResize($w, $h, $s)
   {
      if($this->forceToMaxWidth or $this->forceToMaxHeight) return true;
      if($this->maxWidth > 0 and $this->maxWidth < $w) return true;
      if($this->maxHeight > 0 and $this->maxHeight < $h) return true;
      if($this->maxFileSize > 0 and (($this->maxFileSize*1024) < $s)) return true;
      return false;
   }

   function getFilePrefix()
   {
      $prefix = "";
      $prefix=$prefix.$this->maxWidth."_";
      $prefix=$prefix.$this->maxHeight."_";
      $prefix=$prefix.$this->forceToMaxWidth."_";
      $prefix=$prefix.$this->forceToMaxHeight."_";
      $prefix=$prefix.$this->maxFileSize."_";
      $prefix=$prefix.$this->rotateDegrees."_";
      $prefix=$prefix.$this->cropX1."_";
      $prefix=$prefix.$this->cropY1."_";
      $prefix=$prefix.$this->cropX2."_";
      $prefix=$prefix.$this->cropY2."_";
      return $prefix;
   }
}

