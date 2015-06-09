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

require_once("oscailt_init.inc");


function writeHideList()
{
   global $system_config ;
   $show_reltime = 0;

   $logfile = 'hides_list.log';
   $cols = 6;   

   $log = $system_config->log_store.$logfile;
   if(file_exists($log))
   {
      $logsize = filesize($log);
   }
   else $logsize = 0;

   $chunksize = 5120;
   $chunksize = 3072;
   $startoffset = $chunksize;
   
   if($startoffset > $logsize) $startoffset = $logsize;
   if($chunksize > $logsize) $chunksize = $logsize;

  
   if (!file_exists($log))
   {
      echo "<P class=error>Editorial action file not found.</P>";
      return;
   }
   $fp = fopen($log,"r") ;
   if(is_resource($fp)){
         fseek($fp,-$startoffset,SEEK_END);//last 10k
         $content = fread($fp, $chunksize);
         fclose($fp);
   }
   else
   {
      echo "<P class=error>Error reading entries</P>";
      return;
   }

   ?><TABLE class='admin' border=1><TR>
     <TH class='admin'>Time Hidden</TH><TH class='admin'>Type</TH><TH class='admin'> Id </TH><TH class='admin'>Title</TH><TH class='admin'>Reason</TH></TR>
   <?


   $log_entries = array();
   $entries = array();

   // $current_time = time();

   if(strlen($content) > 0)
   {
         // This will parse them based on a cr-lf separator
         $entries = explode("\n", $content);
         //$messages = explode("\n", $content);
         //foreach($messages as $message)
         //{
	     // debug echo("Entry=" .$message."<BR>");
             //list($t, $m) = explode("\n", $message);
             //if($t != null and $m != null) {
                 //$entries[] = array($t, $m);
             //}
         //}
   }
   if(count($entries) == 0)
   {
       ?><TR class='admin'><TD class='admin' colspan=5><i>No entries found</i></TD></TR><?
       return;
   }
   $entries = array_reverse($entries);


   foreach($entries as $entry)
   {
       // Format is: hide: date("Y-M-d:H:i") story: story_id : story_title reason: 
       //            123456789012345678901234567890123456789012345678901234567890

       $date_part = trim(substr($entry, 5,17));
       if ( strlen($date_part) < 10 ) continue;

       ?><TR><TD class='admin'><?=$date_part?></TD><?
       $rest_of_line = substr($entry, 22,-1);
       $parts = explode(": ", $rest_of_line);
       $token_cnt = 0;

       // 1st part = story or comment
       // 2nd part = item id
       // 3rd part = title
       // 4th part = reason:
       // 5th part = reason text
       $reason_text = strstr($rest_of_line, "reason:");
       $reason_text = substr($reason_text,8);
       foreach($parts as $part)
       {
	    $token_cnt++;

            if($token_cnt  == 4 ) {
		break;
            }
            if($token_cnt  == 3 ) {
		$part = substr($part,0,-6);
            }
            echo "<TD class='admin'>$part</TD>";
       }
       echo "<TD class='admin'>$reason_text</TD>";
       echo "</TR>";
   }
   ?></TABLE> <?
}

   writeHideList();

?>

