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


class PublishMonitorEntry
{
   var $ip;
   var $host_of_ip;
   var $browser_info;
   var $url;
   var $time_posted;
   var $update_host = false;

   function PublishMonitorEntry($url, $ip = false, $time_posted=false, $ip_host="", $browser="")
   {
      $this->url = $url;
      if($ip === false) $this->ip = $_SERVER['REMOTE_ADDR'];
      else $this->ip = $ip;

      if($browser == "" && $time_posted!=false && $ip!=false) $this->browser_info = "Not Available";
      elseif($browser == "") $this->browser_info = $_SERVER['HTTP_USER_AGENT'];
      else $this->browser_info = $browser;

      if($ip_host == "") $this->host_of_ip = gethostbyaddr($_SERVER['REMOTE_ADDR']);
      else $this->host_of_ip = $ip_host;
      if ($this->update_host == true) $this->host_of_ip = gethostbyaddr($this->ip);

      if($time_posted === false) $this->time_posted = time();
      else $this->time_posted = $time_posted;
   }

   function getSerialisedPHP()
   {
      $str = '$recent_publishes[] = new PublishMonitorEntry("'.addslashes($this->url).'", "'.addslashes($this->ip).'", '.$this->time_posted.',"'.addslashes($this->host_of_ip).'","'.addslashes($this->browser_info).'"'.")\n";
      return $str;
   }

}

class PublishMonitor
{
   var $recent_publishes = array();
   var $monitor_size_limit;
   // Detects if it should be switched off automatically on next save.
   var $is_overdue = false;

   function PublishMonitor()
   {
      global $system_config;
      $this->monitor_size_limit = $system_config->monitor_size_limit;

      if ($system_config->publish_monitor_enabled ) {
         if ( (($system_config->publish_monitor_began + $system_config->publish_monitor_due_off) - time()) < 0 )
            $this->is_overdue = true;
      }
   }

   function load($force=false)
   {
      global $system_config;
      if(!$force && count($this->recent_publishes) > 0) return;
      $file_path = $system_config->publish_monitor_cache_store;
      if(file_exists($file_path))
      {
         $fcontents = implode("", file($file_path));
         $recent_publishes = array();
         eval(' ?>'.$fcontents.'<?');
         if(count($recent_publishes) > 0)
         {
            for($i = 0; $i < count($recent_publishes); $i++)
            {
               $recent_publishes[$i]->url = stripslashes($recent_publishes[$i]->url);
            }
            $this->recent_publishes = $recent_publishes;
         }
         $this->discardOldRecords();
      }
   }

   function discardOldRecords()
   {
      while(count($this->recent_publishes) > $this->monitor_size_limit)
      {
         array_shift($this->recent_publishes);
      }
   }

   function add($url)
   {
      $new_entry = new PublishMonitorEntry($url);
      $this->recent_publishes[] = $new_entry;
      $this->discardOldRecords();
   }

   function save()
   {
      global $system_config;

      // If monitor should have been turned off, then do that and empty the array.
      if ($this->is_overdue == true) {
          $this->recent_publishes = array();
	  // Note: All the system variables will been have loaded at the start of the request.
	  $system_config->publish_monitor_enabled = false;
          $system_config->save();
          $logMsg = "oscailt:\tTurned off:\tIP-Monitor:\tN/A:\tSystem automatically turned off monitor as period overdue";
          logMessage($logMsg, "actionlog.txt");

	  // Now generate a message to the Oscailt messaging system
	  require_once("objects/publishstate.inc");

          $editorStatusList = new PublishState();
          $editorStatusList->load();
          $editorStatusList->add("system", "post",time(),"System automatically turned off IP monitor as period overdue");
          $editorStatusList->save();
      }

      $file = $system_config->publish_monitor_cache_store;
      $fp = fopen($file,"w");
      if (flock($fp, LOCK_EX))
      {
         fputs($fp, '<?php '."\n");
         foreach($this->recent_publishes as $one_publish)
         {
            fputs($fp, $one_publish->getSerialisedPHP().";\n");
         }
         // unlocking file
         fputs($fp, '?>'."\n");
         flock($fp, LOCK_UN);
         fclose($fp);
      }
      else echo "<P>failed to get lock on publish cache file $system_config->publish_monitor_cache_store</p>";
   }

}

?>
