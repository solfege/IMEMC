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

class RSSFeedExporter extends FeedExporter
{
   function RSSFeedExporter ($l)
   {
      $this->FeedExporter($l);
   }

   function getContentType()
   {
      return "application/rss+xml";
   }
   function export($version,$feed)
   {
      if($version==1)   return $this->exportVersion1($feed);
      else return $this->exportVersion2($feed);
   }
   function exportVersion1($feed)
   {
      // buffer data
      ob_start();
      echo('<?xml version="1.0" encoding="utf-8"?>');
      ?>
      <rdf:RDF
         xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
         xmlns="http://purl.org/rss/1.0/"
         xmlns:dc="http://purl.org/dc/elements/1.1/">
         <channel rdf:about="<?=$feed->feedLink?>">
            <title><?=htmlspecialchars($feed->title)?></title>
            <link><?=$feed->webLink?></link>
            <description><?=htmlspecialchars($feed->description)?></description>
            <dc:language><?=htmlspecialchars($feed->languageCode)?></dc:language>
            <dc:date><?=$this->formatW3CDate($feed->date,$feed->timezoneOffset)?></dc:date>
            <dc:publisher><?=htmlspecialchars($feed->publisherName)?></dc:publisher>
            <dc:creator><?=htmlspecialchars($feed->publisherName)?></dc:creator>
            <dc:rights><?=htmlspecialchars($feed->license)?></dc:rights>
            <image rdf:resource="<?=htmlspecialchars($feed->logo)?>" />
            <items>
               <rdf:Seq>
               <?
               foreach($feed->items as $item)
               {
                  ?>   <rdf:li rdf:resource="<?=$item->link?>" />
                  <?
               }
               ?>
               </rdf:Seq>
            </items>
         </channel>
         <image rdf:about="<?=htmlspecialchars($feed->logo)?>">
            <title><?=htmlspecialchars($feed->title)?></title>
            <url><?=htmlspecialchars($feed->logo)?></url>
            <link><?=$feed->webLink?></link>
            <dc:description><?=htmlspecialchars($feed->description)?></dc:description>
         </image>
         <?
         foreach($feed->items as $item)
         {
            ?>
            <item rdf:about="<?=htmlspecialchars($item->link)?>">
               <link><?=$item->link?></link>
               <title><?=htmlspecialchars($item->title)?></title>
               <dc:creator><?=htmlspecialchars($item->authorName)?></dc:creator>
               <dc:date><?=$this->formatW3CDate($item->date,$feed->timezoneOffset)?></dc:date>
               <dc:language><?=htmlspecialchars($item->languageCode)?></dc:language>
               <description><?
               if($item->contentType=="html")
               {
                  ?><?=trim(htmlspecialchars($item->summary."<br>\n".$item->content))?><?
               }
               else
               {
                  ?><?=trim(htmlspecialchars($item->summary."\n".$item->content))?><?
               }
               ?></description>
               <?
               if(count($item->categories)>0)
               {
                  ?><dc:subject><?=htmlspecialchars($item->categories[0])?></dc:subject><?
               }
               ?>
            </item>
            <?
         }
         ?>
      </rdf:RDF>
      <?
      //output buffer, and convert
      $exportedFeed=$this->convertToUnicode(ob_get_contents());
      ob_end_clean();
      return $exportedFeed;
   }
   function exportVersion2($feed)
   {
      global $system_config;
      ob_start();
      echo('<?xml version="1.0" encoding="utf-8"?>');
      ?>
      <rss version="2.0" <? if($feed->iTunesTags=="enabled") {?>xmlns:itunes="http://www.itunes.com/dtds/podcast-1.0.dtd" <?} ?> >
         <channel>
            <title><?=htmlspecialchars($feed->title)?></title>
            <description><?=htmlspecialchars($feed->description)?></description>
            <link><?=$feed->siteLink?></link>
            <lastBuildDate><?=$this->formatISO822Date($feed->date,$feed->timezoneOffset)?></lastBuildDate>
            <? 
            if(strlen(trim($feed->languageCode))>0)
            {            
            	?><language><?=htmlspecialchars($feed->languageCode)?></language>
            	<?
            }
            ?>
            <managingEditor><?=htmlspecialchars($feed->publisherEmail)?> (<?=htmlspecialchars($feed->publisherName)?>)</managingEditor>
            <webMaster><?=htmlspecialchars($feed->publisherEmail)?> (<?=htmlspecialchars($feed->publisherName)?>)</webMaster>
            <copyright><?=htmlspecialchars($feed->license)?></copyright>
            <generator><?=htmlspecialchars($feed->generatorName)?> <?=htmlspecialchars($feed->generatorVersion)?></generator>
            <image>
               <url><?=htmlspecialchars($feed->logo)?></url>
               <title><?=htmlspecialchars($feed->title)?></title>
               <link><?=$feed->siteLink?></link>
            </image>
            <?
            if($feed->iTunesTags=="enabled")
            {
            	if(strlen(trim($feed->iTunesSubtitle))>0) 
            	{
            		?><itunes:subtitle><?=htmlspecialchars($feed->iTunesSubtitle)?></itunes:subtitle>
            		<?
            	}
		if(strlen(trim($feed->iTunesAuthor))>0)
		{
			?><itunes:author><?=htmlspecialchars($feed->iTunesAuthor)?></itunes:author>
			<?
		}
		if(strlen(trim($feed->iTunesSummary))>0) 
		{
			?><itunes:summary><?=htmlspecialchars($feed->iTunesSummary)?></itunes:summary>
			<?
		}
		if(strlen(trim($feed->iTunesOwnerName))>0 || strlen(trim($feed->iTunesOwnerEmail))>0) 
		{
			?><itunes:owner>
			<?
			if(strlen(trim($feed->iTunesOwnerName))>0)
			{
				?><itunes:name><?=htmlspecialchars($feed->iTunesOwnerName)?></itunes:name>
				<?
			}
			if(strlen(trim($feed->iTunesOwnerEmail))>0) 
			{
				?><itunes:email><?=htmlspecialchars($feed->iTunesOwnerEmail)?></itunes:email>
				<?
			}
			?></itunes:owner>
			<?
		}
		if(strlen(trim($feed->iTunesCategory))>0 && strlen(trim($feed->iTunesSubcategory))>0) 
		{
			?><itunes:category text="<?=htmlspecialchars($feed->iTunesCategory)?>">
			  <itunes:category text="<?=htmlspecialchars($feed->iTunesSubcategory)?>"/>
			  </itunes:category>
			<?
		}					
		else if(strlen(trim($feed->iTunesCategory))>0)
		{
			?><itunes:category text="<?=htmlspecialchars($feed->iTunesCategory)?>"/>
			<?
		}
		if(strlen(trim($feed->iTunesImage))>0) 
		{
			?><itunes:image href="<?=htmlspecialchars($feed->iTunesImage)?>"/>
			<?
		}					
		if(strlen(trim($feed->iTunesExplicit))>0) 
		{
			?><itunes:explicit><?=htmlspecialchars($feed->iTunesExplicit)?></itunes:explicit>
			<?
		}
		if(strlen(trim($feed->iTunesKeywords))>0) 
		{
			?><itunes:keywords><?=htmlspecialchars($feed->iTunesKeywords)?></itunes:keywords>
			<?
		}
		if(strlen(trim($feed->iTunesBlock))>0) 
		{
			?><itunes:block><?=htmlspecialchars($feed->iTunesBlock)?></itunes:block>
			<?
		}					
            }          
            foreach($feed->items as $item)
            {
            if($item->uniqueId==$item->link) $permaLink="true";
            else $permaLink="false";
            if(strlen(trim($item->authorEmail))==0) $item->authorEmail = $system_config->contact_email_address;
            ?>
            <item>
               <title><?=htmlspecialchars($item->title)?></title>
               <link><?=$item->link?></link>
               <guid isPermaLink="<?=$permaLink?>"><?=$item->uniqueId?></guid>
               <pubDate><?=$this->formatISO822Date($item->date,$feed->timezoneOffset)?></pubDate>
               <?
               if($item->eventDate !=NULL)
               {
                  ?><eventDate><?=$this->formatISO822Date($item->eventDate,$feed->timezoneOffset)?></eventDate><?
               }
               ?>
               <author><?=htmlspecialchars($item->authorName)?></author>
               <description><?
               if($item->contentType=="html")
               {
                  ?><?=trim(htmlspecialchars($item->summary."<br>\n".$item->content))?><?
               }
               else
               {
                  ?><?=trim(htmlspecialchars($item->summary."\n".$item->content))?><?
               }
               ?></description>
               <?
               if(count($item->attachments)>0)
               {
               ?><enclosure url="<?=htmlspecialchars($item->attachments[0]->link)?>" <?
               if($item->attachments[0]->length>0)
               {
               		?> length="<?=htmlspecialchars($item->attachments[0]->length)?>" <?
               }
               ?> type="<?=htmlspecialchars($item->attachments[0]->type)?>" />
               <?
               }
               if($item->commentsLink!=null)
               {
               ?><comments><?=$item->commentsLink?></comments>
               <?
               }
               foreach($item->categories as $category)
               {
                   ?><category><?=htmlspecialchars($category)?></category>
                   <?
               }
               if($feed->iTunesTags=="enabled")
               {
            		?><itunes:author><?=htmlspecialchars($item->authorName)?></itunes:author>
            		<?
		   		?><itunes:summary><?=trim(htmlspecialchars($item->summary))?></itunes:summary>
               		<?								
			if(strlen(trim($feed->iTunesExplicit))>0) 
			{
				?><itunes:explicit><?=htmlspecialchars($feed->iTunesExplicit)?></itunes:explicit>
				<?
			}
			?><itunes:keywords><?
			for($i=0;$i<count($item->categories);$i++)
               		{
				?><?=htmlspecialchars($item->categories[$i])?><?
				if($i!=count($item->categories)-1) echo(", ");
               		}
			?></itunes:keywords>
			<?
			if(strlen(trim($feed->iTunesBlock))>0) 
			{
				?><itunes:block><?=htmlspecialchars($feed->iTunesBlock)?></itunes:block>
				<?
			}	           	
	       }           
               ?>
            </item>
            <?
            }
            ?>
         </channel>
      </rss>
      <?
      //output buffer, and convert
      $exportedFeed=$this->convertToUnicode(ob_get_contents());
      ob_end_clean();
      return $exportedFeed;
   }
}
