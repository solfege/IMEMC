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

class AtomFeedExporter extends FeedExporter
{
   function AtomFeedExporter ($l)
   {
      $this->FeedExporter($l);
   }

   function getContentType()
   {
      return "application/atom+xml";
   }

   function export($version,$feed)
   {
      return $this->exportVersion1($feed);
   }

   function exportVersion1($feed)
   {
      // buffer data
      ob_start();
      echo('<?xml version="1.0" encoding="utf-8"?>');
      ?>
      <feed xmlns="http://www.w3.org/2005/Atom">
         <generator uri="<?=htmlspecialchars($feed->generatorLink)?>" version="<?=htmlspecialchars($feed->generatorVersion)?>"><?=htmlspecialchars($feed->generatorName)?></generator>
         <title><?=htmlspecialchars($feed->title)?></title>
         <subtitle><?=htmlspecialchars($feed->description)?></subtitle>
         <link rel="alternate" type="text/html" href="<?=$feed->siteLink?>"/>
         <link rel="self" type="<?=$this->getContentType()?>" href="<?=$this->self_lnk?>"/>
         <updated><?=$this->formatW3CDate($feed->date,$feed->timezoneOffset)?></updated>
         <author>
            <name><?=htmlspecialchars($feed->publisherName)?></name>
            <?
            if(strlen($feed->publisherEmail)>0)
            {
               ?><email><?=htmlspecialchars($feed->publisherEmail)?></email><?
            }
            ?>
         </author>
         <id><?=$feed->feedLink?></id>
         <logo><?=htmlspecialchars($feed->logo)?></logo>
         <?
         foreach($feed->items as $item)
         {
         ?>
         <entry xml:lang="<?=htmlspecialchars($item->languageCode)?>">
            <title><?=htmlspecialchars($item->title)?></title>
            <link  rel="alternate" type="text/html" href="<?=$item->link?>"/>
            <id><?=$item->uniqueId?></id>
            <updated><?=$this->formatW3CDate($item->date,$feed->timezoneOffset)?></updated>
            <author>
               <name><?=htmlspecialchars($item->authorName)?></name>
               <?
               if(strlen($item->authorEmail)>0)
               {
                  ?><email><?=htmlspecialchars($item->authorEmail)?></email><?
               }
               ?>
            </author>
            <?
            if(strlen($item->summary)==0 && strlen($item->content)>0) 
            {
            	$item->summary=$item->content;
            	if(strlen(strip_tags($item->summary))>80) $item->summary=substr(strip_tags($item->summary),0,80)."...";
            }
            ?>
            <summary type="<?=htmlspecialchars($item->contentType)?>" xml:lang="<?=htmlspecialchars($item->languageCode)?>"><?=htmlspecialchars($item->summary)?></summary>
            <?
            if(strlen($item->content)>0)
            {
            ?><content type="<?=htmlspecialchars($item->contentType)?>" xml:lang="<?=htmlspecialchars($item->languageCode)?>"><?=htmlspecialchars($item->content)?></content>
            <?
            }
            foreach($item->categories as $category)
            {
            ?><category term="<?=htmlspecialchars($category)?>" />
            <?
            }
            foreach($item->attachments as $attachment)
            {
            ?><link rel="enclosure" href="<?=htmlspecialchars($attachment->link)?>" title="<?=htmlspecialchars($attachment->title)?>" <?
            if($attachment->length>0)
            {
            	?> length="<?=htmlspecialchars($attachment->length)?>" <?
            }
            ?> type="<?=htmlspecialchars($attachment->type)?>"/>
            <?
            }
            ?>
         </entry>
         <?
         }
         ?>
      </feed>
      <?
      //output buffer, and convert
      $exportedFeed=$this->convertToUnicode(ob_get_contents());
      ob_end_clean();
      return $exportedFeed;
   }
}
