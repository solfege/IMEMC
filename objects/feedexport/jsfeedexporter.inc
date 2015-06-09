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

// The Javascript Feed Exporter 
class JSFeedExporter extends FeedExporter
{
   function JSFeedExporter ($l)
   {
      $this->FeedExporter($l);
   }

   function getContentType()
   {
      return "application/rss+xml";
   }
   function export($version,$feed)
   {
      // Version doesn't apply. Maybe it will in the future.
      return $this->exportJSHTML($feed);
   }
   function exportJSHTML($feed)
   {
      global $system_config;
      // buffer data
      ob_start();
      header("Content-Type: application/javascript");
      ?>
	function writeLink()
	{
		document.write("<a href='"+this.url+"' title='"+this.summary+". "+this.region+"-"+this.topic+"-"+this.type+"'>"+this.title+"</a> <small><i>"+this.time+"</i></small>");
	}
	function Story(title,summary,region,topic,type,time,url,imageTags)
	{
		this.title=title;
		this.summary=summary;
		this.region=region;
		this.topic=topic;
		this.type=type;
		this.time=time;
		this.url=url;
		this.writeLink=writeLink;
	}
	stories = new Array(<?=count($feed->items)?>);
	<?
	$i=0;
        foreach($feed->items as $item)
	{
		?>
		stories[<?=$i?>] = new Story(	"<?=addslashes($item->title)?>",
						"<?=addslashes(htmlentities(str_replace("\r\n"," ",strip_tags($item->summary))))?>",
						"<?$item->categories[1]?>",
						"<?$item->categories[0]?>",
						"<?$item->categories[2]?>",
						"<?=substr(date("M",($item->date+$system_config->timezone_offset)),0,1)?><?=date("j g:ia",$item->date+$system_config->timezone_offset)?>",
						"<?=$item->link?>");
		<?
		$i++;
	}
	//output buffer, and convert
	$this->writeScriptFunctions();
	$exportedFeed=$this->convertToUnicode(ob_get_contents());
	ob_end_clean();
	return $exportedFeed;
   }

   function writeScriptFunctions()
   {
	?>
	function writeStoryMarquee(width, height, direction, style)
	{
		document.write("<marquee onMouseOver='stop()' onMouseOut='start()'");
		if(style!=null) document.write(" style='"+style+"'");
		if(width!=null) document.write(" width='"+width+"'");
		if(height!=null) document.write(" height='"+height+"'");
		if(direction!=null) document.write(" direction='"+direction+"'");
		document.write(">");
		for(i=0;i<stories.length;i++)
		{
			stories[i].writeLink();
			if(direction=="up" || direction=="down") document.write("<br><br>");
			else document.write("&nbsp;&nbsp;&nbsp;");
		}
		document.write("</marquee>");
	}
	function writeStoryTable(width, height, align, style)
	{
        	document.write("<table ");		
        	if(style!=null) document.write(" style='"+style+"'");		
        	if(width!=null) document.write(" width='"+width+"'");		
        	if(height!=null) document.write(" height='"+height+"'");		
        	if(align!=null) document.write(" align='"+align+"'");		
        	document.write(">");		
        	for(i=0;i<stories.length;i++)		
        	{			
            		document.write("<tr><td>");			
            		stories[i].writeLink();
	    		document.write("</td></tr>");
        	}
		document.write("</table>");
        }
	<?
   }
}
