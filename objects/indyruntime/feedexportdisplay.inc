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

require_once("objects/story.inc");
require_once("objects/comment.inc");
require_once("objects/attachment.inc");
require_once("objects/preferencequery.inc");
require_once("objects/feedexport/feedexporter.inc");
require_once("objects/indyruntime/sitefiltereddisplay.inc");


class feedExportDisplay extends siteFilteredDisplay
{
   function feedExportDisplay($obj_id)
   {
      $this->siteFilteredDisplay($obj_id);
   }


   function processRequest()
   {
      $user_prefs = new PreferenceQuery();
      //error_reporting(E_ERROR);
      $this->setDisplayOptions();
      $user_prefs->readPrefsFromForm($this->user_options, $this->display_options);
      $this->query = new StoryQuery();
      $this->query->loadFormValues();
      $this->onwards_query = $this->query->copy();
      $this->integrateFilters();
      $feed = $this->createFeedFromRequest();
      $feedExporter = $this->createFeedExporterFromRequest();
      if(isset($_REQUEST['action'])) $adminMode=true;
      else $adminMode=false;
      //output feed
      if($adminMode)
      {
        echo("<pre>".htmlspecialchars(trim($feedExporter->export($this->getDisplayOption("feedversion"),$feed)))."</pre>");
      }
      else
      {
         header("Content-Type: ".$feedExporter->getContentType());
         echo(trim($feedExporter->export($this->getDisplayOption("feedversion"),$feed)));
      }
   }


   // title, description and categories may be null
   function createFeed($title,$description,$objects,$depth,$format)
   {
      global $system_config, $pageTopic, $pageRegion, $pageType, $pageLanguage;
      $feed = new Feed();


      if($title=="")
      {
         $titlePostfix="";
         if($pageTopic!=null) $titlePostfix = $titlePostfix." ".$pageTopic->topic_name;
         if($pageRegion!=null) $titlePostfix = $titlePostfix." ".$pageRegion->region_name;
         if($pageType!=null) $titlePostfix = $titlePostfix." ".$pageType->type_name;


         $feed->title = $system_config->site_short_name.$titlePostfix;
      }
      else
      {
         $feed->title = $title;
      }


      if($description!="")
      {
         $feed->description = $description;
      }
      else
      {
         $feed->description = $system_config->site_description;
      }


      $feed->generatorName=$system_config->software_name;
      $feed->generatorVersion=$system_config->software_version;
      $feed->generatorLink="http://www.indymedia.ie/oscailt/";


      $feed->siteLink="http://".$system_config->site_url;
      $feed->feedLink=$feed->siteLink."/".$this->getFilterLink();
      $feed->webLink=$feed->siteLink."/".$this->getFilterLink($this->site_settings['featureobject']);


      $feed->iTunesTags=$this->getDisplayOption("itunestags");
      $feed->iTunesImage = $this->getDisplayOption("itunesimage");
      if($feed->iTunesImage == "" && file_exists("./graphics/itunesimage.jpg")) $feed->iTunesImage = $feed->siteLink."/graphics/itunesimage.jpg";
      $feed->iTunesExplicit=$this->getDisplayOption("itunesexplicit");
      $feed->iTunesCategory=$this->getDisplayOption("itunescategory");
      $feed->iTunesSubcategory=$this->getDisplayOption("itunessubcategory");
      $feed->iTunesAuthor=$this->getDisplayOption("itunesauthor");
      $feed->iTunesOwnerName=$this->getDisplayOption("itunesownername");
      $feed->iTunesOwnerEmail=$this->getDisplayOption("itunesowneremail");
      $feed->iTunesSummary=$this->getDisplayOption("itunessummary");
      $feed->iTunesSubtitle=$this->getDisplayOption("itunessubtitle");
      $feed->iTunesKeywords=$this->getDisplayOption("ituneskeywords");
      $feed->iTunesBlock=$this->getDisplayOption("itunesblock");


      $logo = $this->getDisplayOption("feedlogo");
      if($logo == "" && file_exists("./graphics/feedlogo.gif")) $logo = "/graphics/feedlogo.gif";
      $icon = $this->getDisplayOption("feedicon");
      if($icon == "" && file_exists("./favicon.ico")) $icon = "/favicon.ico";
      $feed->logo=$feed->siteLink.$logo;
      $feed->icon=$feed->siteLink.$icon;


      $feed->publisherName=$system_config->site_short_name;
      $feed->publisherLink=$system_config->site_url;
      $feed->publisherEmail=$system_config->contact_email_address;


      $licence = $this->getDisplayOption('feedlicence');
      if($licence == "") $licence="Open Content License, http://www.opencontent.org/";
      $feed->license=$licence;


      if(count($objects)>0) $feed->date=$objects[0]->time_posted;
      else $feed->date=time();


      $feed->timezoneOffset=$system_config->timezone_offset;


      if($pageTopic!=null) array_push($feed->categories,$pageTopic->topic_name);
      if($pageRegion!=null) array_push($feed->categories,$pageRegion->region_name);
      if($pageType!=null) array_push($feed->categories,$pageType->type_name);


      // finally convert all oscailt objects into proper news feed items.
      $itemTitles = array();
      foreach($objects as $object)
      {
          $feedItem = $this->createFeedItem($object, $depth, $format);
          // now do some jiggery pokery to insert numbers after items with same title
          // this happens most on file centric feeds and looks lame in rss readers without adding numbers
	  if(isset($itemTitles[$feedItem->title]))
	  {
          	$duplicateTitleIndex = $itemTitles[$feedItem->title];
	  }
	  else
	  {
	        $duplicateTitleIndex = 0;
	  }
          $duplicateTitleIndex++;
          $itemTitles[$feedItem->title]=$duplicateTitleIndex;
          if($duplicateTitleIndex==2)
          {
            for($i=0;$i<count($feed->items);$i++)
            {
               $findItem = $feed->items[$i];
               if($findItem->title==$feedItem->title)
               {
                  $findItem->title=$findItem->title." [1]";
                  $feed->items[$i] = $findItem;
                  break;
               }
            }
          }
          if($duplicateTitleIndex>1) $feedItem->title=$feedItem->title." [".$duplicateTitleIndex."]";
          //push item onto array
          array_push($feed->items, $feedItem);
      }


      if($pageLanguage!=null)
      {
         $feed->languageName=$pageLanguage->language_name;
         $feed->languageCode=$pageLanguage->language_code;
      }
      else if(count($feed->items)>0)
      {
          $item=$feed->items[0];
          $feed->languageName=$item->languageName;
         $feed->languageCode=$item->languageCode;
      }
      else
      {
          $feed->languageName="en";
         $feed->languageCode="English";
      }


      return $feed;
   }


   function createFeedFromQuery($query,$depth,$format,$parseContent)
   {
      global $system_config,$regionList,$topicList,$typeList,$languageList;


      // add feed items
      $objects = array();
      // if story specified make feed items from story and story comments
      if($query->story_id>0)
      {
         $story = new Story();
         $story->story_id=$query->story_id;
         $story->load();
         $story->unformat();
         $title = $story->story_title;
         if(strlen($story->story_subtitle)>0) $description=$story->story_subtitle;
         else $description=$story->story_summary;
         $objects = $story->getComments();
         // if file centric specified flatten comments into their attachments
         if($query->attachment_query)
         {
            $attachments=array();
            foreach($objects as $object)
            {
               if(strcasecmp(get_class($object), "attachment") == 0) array_push($attachments,$object);
               else
	       { 	
		  $rslts = $object->getAttachments();
		  if(is_array($rslts))
		  { 
			$attachments=array_merge($attachments,$rslts);
		  }
	       }
            }
            $objects=$attachments;
         }
      }
      // otherwise make feed items from stories
      else
      {
         $title = $this->getDisplayOption("feedtitle");
         $description = $this->getDisplayOption("feeddescription");
         $objects = $query->execute();
      }


      // tidy up attachments
      if($query->attachment_query)
      {
         // add in any dummy parsed attachments first
         if($parseContent)
	 {
	    $results = $this->extractAttachmentsFromQueryResults($query);
            if(is_array($results))
	    {
		 $objects=array_merge($objects,$results);
	    }
	 }
         // remove duplicates
         $objects=$this->removeDuplicateAttachments($objects);
         // sort by date
         usort($objects,"attachmentDateCompare");
         // trim below results
         if(count($objects)>$query->results_limit) $objects = array_slice ($objects, 0, $query->results_limit);
      }


      return $this->createFeed($title,$description,$objects,$depth,$format);
   }


   function removeDuplicateAttachments(&$inputAttachments)
   {
      $attachments = array();
      foreach($inputAttachments as $object)
      {
         $found=false;
         foreach($attachments as $check)
         {
            if($check->attachment_file==$object->attachment_file)
            {
               $found=true;
               break;
            }
         }
         if(!$found) array_push($attachments,$object);
      }
      return $attachments;
   }


   function extractAttachmentsFromQueryResults($query)
   {
      $attachments=array();
      if(count($query->filetypes)>0)
      {
         $query->selectFullMode();
         $query->attachment_query=false;


         $objects = $query->execute();
         foreach($objects as $object)
         {
            $results = $this->extractAttachmentsFromObjectLinks($object,$query->filetypes);
            if(is_array($results))
	    {
            $attachments=array_merge($attachments,$results);
	    }
            if(strcasecmp(get_class($object), "story") == 0)
            {
               foreach($object->getComments() as $comment)
               {
                  $results = $this->extractAttachmentsFromObjectLinks($comment,$query->filetypes);
	          if(is_array($results))
		  {
                  $attachments=array_merge($attachments,$results);
               }
            }
         }
      }
      }
      return $attachments;
   }


   function extractAttachmentsFromObjectLinks($object,$filetypes)
   {
      if(strcasecmp(get_class($object), "story") == 0) $text=$object->story_summary." ".$object->story_content." ".$object->related_link;
      else if(strcasecmp(get_class($object), "comment") == 0) $text=$object->comment_content." ".$object->related_link;
      else return;
      $attachments=array();
      $tag = "/<a[^>]+href=[\"']?([^\"' ]+)[\"']?[>|\s.*>]/i";
      preg_match_all($tag, $text, $matches);
      foreach($matches[1] as $match)
      {
         foreach($filetypes as $filetype)
         {
            if(strrchr(strtolower($match),".")==".".strtolower($filetype))
            {
               array_push($attachments,$this->createAttachmentFromLink($match,$object));
            }
         }
      }
      return $attachments;
   }


   function createFeedFromRequest()
   {
      global $system_config;


      $depth = strtolower($this->getDisplayOption('feeddepth'));
      if($depth != 'content') $depth == 'summary';


      $format = strtolower($this->getDisplayOption('feedformat'));
      $format = str_replace(" ", "", $format);


      if($this->getDisplayOption('filecentric') == 'true') $fileCentric=true;
      else $fileCentric=false;

      // Special case for Javascript feeds because there are no attachments for it. And set it to false.
      if(!strcasecmp($this->getDisplayOption('feedtype'),"Javascript")) 
      {
	 $fileCentric=false;
         $this->query->attachment_query=false;
      }

      if($this->getDisplayOption('parsecontent') == 'true' && $fileCentric) $parseContent=true;
      else $parseContent=false;


      $filetypes = explode(",", $this->getDisplayOption('filetypes'));
      if(is_array($filetypes) && $fileCentric)
      {
         foreach($filetypes as $filetype)
         {
            array_push($filetypes,addslashes(strtolower(trim($filetype))));
         }
      }


      //$query = new StoryQuery();
      //$query->loadFormValues();
      if($depth=="content") $this->query->selectFullMode();
      else $this->query->selectSummaryMode();
      //numeric value from options with boundary check
      $feedlength = $this->getDisplayOption("feedlength");
      if($feedlength <= 0 or $feedlength > 200) $feedlength = 50;
      $this->query->results_limit=$feedlength;


      // if a detailed search is on don't use excludes
      $this->query->use_excludes=true;
      if($fileCentric)
      {
         $this->query->attachment_query=true;
         if(is_array($filetypes)) $this->query->filetypes=$filetypes;
      }
      return $this->createFeedFromQuery($this->query,$depth,$format,$parseContent);
   }


   function createFeedItem($object, $depth, $format)
   {
      global $system_config;
      $link_base = "http://".$system_config->site_url."/";
      $summaryLength=$this->getDisplayOption("feedsummarylength");
      if($summaryLength <= 0 or $summaryLength > 1000) $summaryLength = 500;


      $item = new FeedItem();
      if(strcasecmp(get_class($object), "attachment"))
      {
      $item->authorName=$object->author_name;
      $item->authorEmail=$object->author_email;
      }
      $item->date=$object->time_posted;
      $item->categories=array();


      if($format=="html" || $format=="thumbnailtext") $item->contentType="html";
      else $item->contentType="plain";


      if($format=="html" || $format=="thumbnailtext") $newline="<BR>\n";
      else $newline="\n";
      if(strcasecmp(get_class($object), "story") == 0)
      {
         $object->splitFeatureContent();
         if($format!="html") $object->unformat();
         if($format=="thumbnailtext") $object->format();


         $item->link = $link_base.$this->getStoryLink($object->story_id);
         $item->commentsLink=$item->link."#comments";
         $item->uniqueId=$item->link;


         $topic = $object->getTopic();
         array_push($item->categories,$topic->topic_name);
         $region = $object->getRegion();
         array_push($item->categories,$region->region_name);
         $type = $object->getType();
         array_push($item->categories,$type->type_name);
         $language = $object->getLanguage();
         $item->languageCode=$language->language_code;
         $item->languageName=$language->language_name;
         $item->title=$object->story_title;

	 // For events add in the event date. Bit of a hack to use the known db Id for event. Fixed now.
         if ($type->type_id == $system_config->event_type_id) $item->eventDate = $object->event_time;
	 else $item->eventDate = NULL;

         if($format=="thumbnailtext") $item->summary=$this->getThumbnailHTML($object);
         if($depth=="summary")
         {
            if(strlen($object->story_subtitle)>0) $item->summary=$item->summary.$object->story_subtitle.$newline.$object->story_summary;
            else $item->summary=$item->summary.$object->story_summary;
            if($format=="plaintext") $item->summary=substr($item->summary,0,$summaryLength)." ...";
         }
         else if($depth=="content")
         {
            if(strlen($object->story_subtitle)>0) $item->summary=$item->summary.$object->story_subtitle.$newline.$object->story_summary;
            else $item->summary=$item->summary.$object->story_summary;
            $item->content=$object->story_content.$newline.$object->story_content_extra;
         }
         foreach($object->getAttachments() as $attachment)
         {
            array_push($item->attachments, $this->createFeedItemAttachment($attachment, $depth, $format));
         }
      }
      else if(strcasecmp(get_class($object), "comment") == 0)
      {
         if($format!="html") $object->unformat();
         $parentStory=$object->getParentStory();


         $item->link = $link_base.$this->getStoryLink($object->story_id)."#comment".$object->comment_id;
         $item->uniqueId=$item->link;


         $topic = $parentStory->getTopic();
         array_push($item->categories,$topic->topic_name);
         $region = $parentStory->getRegion();
         array_push($item->categories,$region->region_name);
         $type = $parentStory->getType();
         array_push($item->categories,$type->type_name);
         $language = $parentStory->getLanguage();
         $item->languageCode=$language->language_code;
         $item->languageName=$language->language_name;
         $item->title=$object->comment_title;
         if($depth=="summary")
         {
            if($format=="thumbnailtext") $item->summary=$this->getThumbnailHTML($object);
            $item->summary=$item->summary.$object->comment_content;
            if($format=="plaintext") $item->summary=substr($item->summary,0,$summaryLength)." ...";
         }
         else if($depth=="content")
         {
            if($format=="thumbnailtext") $item->content=$this->getThumbnailHTML($object);
            $item->content=$item->content.$object->comment_content;
         }
         foreach($object->getAttachments() as $attachment)
         {
            array_push($item->attachments, $this->createFeedItemAttachment($attachment, $depth, $format));
         }
      }
      else if(strcasecmp(get_class($object), "attachment") == 0)
      {
         $parentStory = new Story();
         $parentStory->story_id=$object->story_id;
         $parentStory->load();
         $parentStory->unformat();
         $parentComment = new Comment();
         $parentComment->comment_id=$object->comment_id;
         if($object->comment_id>0) $parentComment->load();
         $parentComment->unformat();


         if($object->attachment_id==null) //fake attachment extracted from link
         {
            if($object->comment_id>0) $item->link=$link_base.$this->getStoryLink($object->story_id)."#comment".$object->comment_id;
            else $item->link=$link_base.$this->getStoryLink($object->story_id);
            $item->uniqueId=$object->dummy_link_url;
         }
         else
         {
            $item->link=$link_base.$this->getStoryLink($object->story_id)."#attachment".$object->attachment_id;
            $item->uniqueId=$item->link;
         }


         $topic = $parentStory->getTopic();
         array_push($item->categories,$topic->topic_name);
         $region = $parentStory->getRegion();
         array_push($item->categories,$region->region_name);
         $type = $parentStory->getType();
         array_push($item->categories,$type->type_name);
         $language = $parentStory->getLanguage();
         $item->languageCode=$language->language_code;
         $item->languageName=$language->language_name;


         if(strlen($object->description)>0) $item->title=$object->description;
         if($object->comment_id>0)
         {
            if($item->title==null) $item->title=$parentComment->comment_title;
            $item->authorName=$parentComment->author_name;
            $item->authorEmail=$parentComment->author_email;
            $item->summary=$parentComment->comment_content;
         }
         else
         {
            if($item->title==null) $item->title=$parentStory->story_title;
            $item->authorName=$parentStory->author_name;
            $item->authorEmail=$parentStory->author_email;
            $item->summary=$parentStory->story_summary;
         }
         array_push($item->attachments, $this->createFeedItemAttachment($object, $depth, $format));
      }


      // handle oscailt macros : expand for html, strip for plain text or thumbnailtext
      if($format=="html")
      {
         $item->summary=substituteOscailtMacros($item->summary);
         $item->content=substituteOscailtMacros($item->content);
      }
      else
      {
         $item->summary=stripOscailtMacros($item->summary);
         $item->content=stripOscailtMacros($item->content);
      }


      return $item;
   }


   function getThumbnailHTML(&$object)
   {
      global $system_config, $pseudo_directory_offset;
      $images = $object->getImageAttachments();
      if(is_array($images))
      {
         if(count($images)>0)
         {
            $transformedImage_info=$images[0]->getTransformedImageURL($system_config->story_summary_thumbnail);
            if($transformedImage_info)
            {
               $transformedImageURL = $transformedImage_info[0];
               if(strcasecmp(substr($transformedImageURL,0,4),"http"))  $transformedImageURL= "http://".$system_config->site_url."/".$transformedImageURL;
               return "<img ".$transformedImage_info[2]." align=\"left\" src=\"".$transformedImageURL."\" title=\"".$images[0]->getDisplayName()."\" alt=\"".$images[0]->getDisplayName()."\">";
            }
         }
      }
      return "";
   }


   function createFeedItemAttachment($object, $depth, $format)
   {
      global $system_config;
      $attachment = new FeedItemAttachment();
      if(strlen($object->description)>0) $attachment->title=$object->description;
      else $attachment->title=basename($object->attachment_file);
      if(isset($object->dummy_link_url) && $object->dummy_link_url!=null) $attachment->link=$object->dummy_link_url;
      else
      {
         $attachment->link=$object->getURL();
      if(strcasecmp(substr($attachment->link,0,4),"http"))  $attachment->link= "http://".$system_config->site_url."/".$attachment->link;
      }
      $attachment->type=$this->getMimeType($object->attachment_file);
      if(file_exists($system_config->attachment_store.$object->attachment_file))
      {
         $attachment->length=@filesize($system_config->attachment_store.$object->attachment_file);
      }
      return $attachment;
   }


   //create dummy attachment to hold extracted link
   function createAttachmentFromLink($link,$parent)
   {
      global $system_config;
      $attachment = new Attachment();
      $attachment->story_id=$parent->story_id;
      if(strcasecmp(get_class($parent), "comment") == 0) $attachment->comment_id=$parent->comment_id;
      $attachment->time_posted=$parent->time_posted;
      $attachment->dummy_link_url=$link;
      $attachment->attachment_file=$link;
      return $attachment;
   }


   function createFeedExporter($feedType)
   {
      // determine type of feed requested, default to RSS
      global $system_config;
      $self_lnk = "http://".$system_config->site_url."/".$this->getFilterLink();
      if(isset($_REQUEST['feedtype']) && $_REQUEST['feedtype'] == 'atom')
      {
         $self_lnk .= "&amp;feedtype=atom";
      }
      if(!strcasecmp($feedType,"RSS")) return new RSSFeedExporter($self_lnk);
      else if(!strcasecmp($feedType,"Atom")) return new AtomFeedExporter($self_lnk);
      else if(!strcasecmp($feedType,"Javascript")) return new JSFeedExporter($self_lnk);
      else return new RSSFeedExporter();
   }

   function createFeedExporterFromRequest()
   {
      return $this->createFeedExporter($this->getDisplayOption("feedtype"));
   }

   function getMimeType($filename)
   {
         global $mimeTypes;
         $mimeType = $mimeTypes[strtolower(strrchr($filename, '.'))];
         if($mimeType == null) $mimeType = "application/unknown";
         return $mimeType;
   }
}



// array sorting fxn
function attachmentDateCompare($a, $b)
{
   global $system_config;
   //print_r($a);
   if ($b->time_posted == $a->time_posted) return 0;
   return ($b->time_posted < $a->time_posted) ? -1 : 1;
}
?>
