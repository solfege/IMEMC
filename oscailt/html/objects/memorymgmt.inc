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

define ('STANDARD_MEM', 0);
define ('HOURLY_MEM', 1);
// This is the number of bytes at the start allocated to storing the byte count in the memory block
// It should match the formatting. i.e. - sprintf("%06d", 0)
define ('DATA_OFFSET', 6);

define ('MEM_MODE_CREATE',1);
define ('MEM_MODE_WRITE' ,2);
define ('MEM_MODE_READ'  ,3);

define ('SEM_KEY_ID'  ,534713);

$semaphore_id = 0;
// The sem_xxx functions are not available in Windows. So uncomment these for Windows installations.
/*

$sem_internal =0;
function sem_get($var)
{
    return 1;
}
function sem_release($var)
{
    global $sem_internal;
    $sem_internal--;
    return 1;
}
function sem_remove($var)
{
    return 1;
}
function sem_acquire($var)
{
    global $sem_internal;
    $sem_internal++;
    return 1;
}

*/

// This should be called near the start of any request but after the configuration has been read.
function GetSemaphoreId()
{
    global $system_config, $semaphore_id;
    
    if ($system_config->memory_mgmt_installed != true) return;
    if ($system_config->memory_mgmt_activated == false) return;

    // If it is zero then get it, otherwise we already have it...
    if ($semaphore_id == 0) {
        // Make the call to get the semaphore, then use acquire and release for locking 
        // and remove at the end of the script. Default permission = 0666. Auto-release is default.
        $semaphore_id = sem_get(SEM_KEY_ID, 1, 0666, 1);
        if ($semaphore_id === FALSE ) {
	    // Log it and temporarily deactivate but just for this request.
            $system_config->memory_mgmt_activated = false; 
            if ($system_config->user_error_reporting > 1) 
	        logMessage("Error getting semaphore id for semaphore key ".SEM_KEY_ID." for request ".$_SERVER['REQUEST_URI'], "sitelog.txt");
        } 
    } 
}
// Used to drop the semaphore at the end of the request.
function RemoveSemaphoreId()
{
    // Autorelease is now enabled.
    return;

    // Code below is no longer used. First major site test was showing a lot of errors removing semaphore
    // It may have been because Apache will process multiple requests in the same process and the semaphore
    // is got per process thus removing it from other threads. Looks like the auto-release means PHP does a
    // proper cleanup and release. 
    global $system_config, $semaphore_id;
    
    if ($system_config->memory_mgmt_installed != true) return;
    if ($system_config->memory_mgmt_activated == false) return;

    // If it is zero then it is already removed.
    if ($semaphore_id != 0) {
        // Make the call to get the semaphore, then use acquire and release for locking 
        // and remove at the end of the script.
        $remove_res = sem_remove($semaphore_id);
        if ($remove_res === FALSE ) {
            if ($system_config->user_error_reporting > 1) 
                logMessage("Error removing semaphore id ".$semaphore_id." for request ".$_SERVER['REQUEST_URI'], "sitelog.txt");
        } else {
            if ($system_config->user_error_reporting > 1) 
                logMessage("Successfull removal semaphore id ".$semaphore_id." for request ".$_SERVER['REQUEST_URI'], "sitelog.txt");
        }
        $semaphore_id = 0;
    } 
}

class MemoryStore
{
      var $shm_id;
      var $mem_id;
      var $debug_on;
      var $hourly_mode;
      var $windows_bug;
      var $hour; 
      var $index;
      var $reset; 
      var $safety_check;
      var $no_hours; 
      var $shm_size;
      var $shm_data;
      var $shm_leading;
      var $shm_trailing ="";

      function MemoryStore($mem_id, $mem_type=STANDARD_MEM, $mem_mode=MEM_MODE_READ, $mem_size = 100, $reset= false)
      {
         $this->index = 0;
         $this->no_hours = 24; 
	 $this->safety_check = false;
         // $this->no_hours = 5; 

         if ($mem_type == HOURLY_MEM) {
            // There can be many calls but this is set only once externally to this class.
            global $current_hour;

            $this->hourly_mode = true;
            $this->hour = $current_hour;
            $mem_size = $this->no_hours *  10 + 5;
         } else {
            $this->hourly_mode = false;
            $this->hour = 0;
         }

         $this->debug_on = true;
         $this->debug_on = false;
         $this->mem_id   = $mem_id;
         $this->shm_size = $mem_size;
         $this->reset    = $reset;
         $this->shm_id   = 0;
         if ($mem_mode == MEM_MODE_READ) {
             // No need to specify size when already created. Same for permissions.
             $this->shm_id = shmop_open($mem_id, "a", 0, 0);

         } else if ($mem_mode == MEM_MODE_WRITE) {
             // Open for write .... manual says to set permissions to 0 for open for write.
             $this->shm_id = @shmop_open($mem_id, "w", 0, 0);

         } else if ($mem_mode == MEM_MODE_CREATE) {
             // When creating, we want to make sure it does not exist already so we attempt to read it first.
             // And we should get an error which means it does not exist.
             $this->shm_id = @shmop_open($mem_id, "a", 0644, $mem_size);
	     if ($this->shm_id && $this->safety_check == true) {
                 // Oh dear someone else is already using this... so close it quick.
                 shmop_close($this->shm_id);
                 $this->shm_id = 0;
             } else {
                 $this->shm_id = shmop_open($mem_id, "c", 0644, $mem_size);
                 if ($this->shm_id) {
                     if ($this->reset == true) $this->initMemoryNumber();
                 } else {
                     $this->shm_id = 0;
                 }
             }
         } else {
             echo("Unknown mode ".$mem_mode." to open memory object ".$mem_id." with <br>");
         }
      }

      function initMemoryNumber()
      {
          // Memory is initialed when activated. It can be done by generating a formatted string.
          // Hourly mode: xx:nnnnnn xx:nnnnnn .....  x 24  Standard Mode: xxxxxx: -count. set to zero 
          if ($this->debug_on == true) echo "Calling reset memory <BR>";

	  if ($this->hourly_mode == true) {
             $leading_str = sprintf("%02d:%06d ", $this->hour, 0);
             $shm_str = $leading_str . $this->shm_trailing;
             $shm_page_count = "";
             for ($k = 0; $k < $this->no_hours; $k++) {
                 // Just do from 00 to 23 No need to do stuff with hour placement
                 // $shm_page_count .= sprintf("%02d:%06d ", ($this->hour+$k), 0);
                 $shm_page_count .= sprintf("%02d:%06d ", $k, 0);
             }
             $shm_bytes_written = shmop_write($this->shm_id, $shm_page_count, 0);
             if($shm_bytes_written != strlen($shm_page_count)) {
                 echo "writeMemoryNumber: Couldn't write the entire length of data<BR>";
             }
             if ($this->debug_on == true) echo("Initialized number: [".$shm_page_count."]<BR>");
          } else {
             $shm_page_count = sprintf("%06d", 0);
             if ($this->debug_on == true) echo("Writing number: [".$shm_page_count."]<BR>");

             $shm_bytes_written = shmop_write($this->shm_id, $shm_page_count, 0);
             if($shm_bytes_written != strlen($shm_page_count)) {
                 echo "initMemoryNumber: Couldn't write the entire length of data<BR>";
             }
          }
      }

      function setSize()
      {
	 // Get size and store it in the object.
	 $t_size = shmop_size($this->shm_id);
         if ($t_size > 0 ) $this->shm_size = $t_size;
      }
      function readMemory()
      {
	 // 1000000,9990000
	 if ($this->shm_id == null) return "";

	 $this->shm_data = shmop_read($this->shm_id, 0, $this->shm_size);
         if(!$this->shm_data) {
              echo "Couldn't read from shared memory block\n";
         }
         // $this->debug_on =true;
         if ($this->debug_on==true) echo "Data in shared memory id ".$this->mem_id." was: [".$this->shm_data."]<BR>";
         if ($this->debug_on==true) echo "Data length was: [".strlen($this->shm_data)."]<BR>";

         $this->shm_data = trim($this->shm_data);
	 $length = substr($this->shm_data, 0, DATA_OFFSET);
	 $this->shm_data = substr($this->shm_data, DATA_OFFSET,$length);

         if ($this->debug_on == true) echo "Lenght read: ".$length." Trimmed data :[".$this->shm_data."]<BR>";

	 return $this->shm_data;
      }
      
      function writeMemory($mem_data)
      {
         // Format: LEN 6 bytes followed by data up to max size
         $length_str = sprintf("%06d",strlen($mem_data));
         $bytes_written = shmop_write($this->shm_id, $length_str . $mem_data, 0);

         if($bytes_written != (strlen($mem_data)+DATA_OFFSET)) {
              $shm_size = shmop_size($this->shm_id);
              echo "writeMemory: Couldn't write ".strlen($mem_data)+DATA_OFFSET." bytes. Max is ".$shm_size."<BR>Data was: [".$mem_data."]<BR>";
         }
      }

      function readMemoryNumber($read_all = false)
      {
	 if ($this->shm_id == null) return "";

	 $this->shm_data = shmop_read($this->shm_id, 0, $this->shm_size);
         if(!$this->shm_data) {
              echo "Couldn't read from shared memory block\n";
         }
         if ($this->debug_on==true) echo "Data in shared memory id ".$this->mem_id." was: [".$this->shm_data."]<BR>";
         if ($this->debug_on==true) echo "Data length was: [".strlen($this->shm_data)."]<BR>";

	 if ($this->hourly_mode == true) {
             // In hourly mode decode out the entire set of 24 readings, then using the current hour pull out
             // the correct value. There are two modes of reading. Return reading for current hour or all data.

	     // But do store the index to the current hour, in case of a write op later. (note index is 1 based)
	     if ($this->hour >= 0 && $this->hour < 24 ) {
	         $this->index = ($this->hour + 1);
	     }
             $t_start = 0;
             if ($read_all == true) {
                 $data_cx_hr = array();

                 for ($k = 0; $k < $this->no_hours; $k++) {
                     $t_data  = substr($this->shm_data, $t_start, 10);
                     list($t_hr, $t_ct) = sscanf($t_data,"%02d:%06d ");
		     //echo("thr=".$t_hr." tct = ".$t_ct."<BR>");
                     $data_cx_hr[$t_hr] = $t_ct;
                     $t_start = $t_start + 10;
                 }
                 return $data_cx_hr;

             } else {
	         // Reading is by size x current hour. Beware hour is 0...23
	         $t_start = $this->hour * 10;
                 $t_data  = substr($this->shm_data, $t_start, 10);
                 list($t_hr, $t_number) = sscanf($t_data,"%02d:%06d ");
		 // Should add a check to confirm that t_hr really matches $this->hour
             }
         } else {
             $this->shm_data = trim($this->shm_data);
             if ($this->debug_on == true) echo "Trimmed data is [".$this->shm_data."]<BR>";
             $t_number = $this->shm_data;
         }

         if ($this->debug_on == true) echo "Returning number: [".$t_number."]<BR>";
	 return $t_number;
      }

      function incMemoryNumber()
      {
	 $page_count = $this->readMemoryNumber();
         if ($page_count =="") $page_count = 1;
         else $page_count++;

	 // Do not output debug here as it is probably before headers are sent.
         // echo "incMemoryNumber: Count is now: [".$page_count."]<BR>";
         $this->writeMemoryNumber($page_count);

         return $page_count;
      }

      function incMemoryCounter()
      {
	 $page_count = $this->readMemoryNumber();
         if ($page_count =="") {
             // Update the article list tracker.
             $ArticleListMem = new MemoryStore(404, STANDARD_MEM, MEM_MODE_WRITE);
             $last_list = $ArticleListMem->readMemory();
             echo("Prev List Mem: [".$last_list . "]<BR>");
             echo("This List Mem: [".$this->mem_id . "]<BR>");
             echo("Full List Mem: [".$last_list. " ".$this->mem_id . "]<BR>");
             $ArticleListMem->writeMemoryAsList($last_list, $this->mem_id, 6);
             $ArticleListMem->close();

             $page_count = 1;
         } else {
             $page_count++;
         }
         echo "incMemoryCount: Count is now: [".$page_count."]<BR>";
         $this->writeMemoryNumber($page_count);

         return $page_count;
      }

      function writeMemoryAsList($mem_list, $new_data, $max_size = 10)
      {
         $tmp_array = explode(" ",$mem_list);
         $list_count = count($tmp_array);

         if ($this->debug_on == true) echo("No elements in list: ".$list_count ."<BR>");

	 // Add one on the end of the array
         array_push($tmp_array, $new_data);
      
	 // Pop one off the front of the array.
         if ($list_count > $max_size ) array_shift($tmp_array);
         $new_list = implode(" ",$tmp_array);

	 // Need to get length at start and then pad with blanks to overwrite any extra bits
         $this->writeMemory($new_list);
      }
      

      // Treat it as an integer
      function writeMemoryNumber($number_value)
      {
          // Lets write a test string into shared memory. Use sscanf to read.
          // generate formatted string
          // Hourly mode: xx:nnnnnn xx:nnnnnn .....  x 24
	  if ($this->hourly_mode == true) {
             //$this->debug_on = true; 
             if ($this->debug_on == true) {
                 echo "writeMemoryNumber: hourly mode for " .$number_value ." hrs ". $this->no_hours."<BR>";
                 echo "writeMemoryNumber: current hour " .$this->hour ." Index ". $this->index."<BR>";
	     }
             $leading_str = sprintf("%02d:%06d ", $this->hour, $number_value);
             $shm_str = $leading_str . $this->shm_trailing;
             $shm_page_count = "";
	     $start_1_pos = 0;
	     $start_2_pos = 0;
             $shm_page_count .= sprintf("%02d:%06d ", $this->hour, $number_value);
             if ($this->index == 1) {
                 // Beginning -the first.
                 $start_1_pos = ($this->index * 10);
                 $mem_string = $shm_page_count . substr($this->shm_data, $start_1_pos);
                 if ($this->debug_on == true) echo "[". substr($this->shm_data, $start_1_pos) ."] <BR>";
	     } else if ($this->index == $this->no_hours) {
                 $mem_string = substr($this->shm_data, 0, (($this->index-1) * 10)) . $shm_page_count;
	     } else {
                 $start_1_pos = (($this->index - 1) * 10);
                 $start_2_pos = $start_1_pos + 10;
                 $mem_string = substr($this->shm_data, 0, $start_1_pos) . $shm_page_count . substr($this->shm_data, $start_2_pos);
             }

             if ($this->debug_on == true) 
                 echo "writeMemoryNumber: Hour-mode writing [".$mem_string ."] for id ". $this->mem_id ."<BR>";
             $shm_bytes_written = shmop_write($this->shm_id, $mem_string, 0);
             if($shm_bytes_written != strlen($mem_string)) {
                 echo "writeMemoryNumber: Hour-mode Couldn't write the entire length (".strlen($mem_string).") of data. Max (".$shm_bytes_written.")<BR>";
                 echo "writeMemoryNumber: String [". $mem_string ."]<BR>";
             }
          } else {
             $shm_page_count = sprintf("%06d", $number_value);
             if ($this->debug_on == true) echo("Writing number: [".$shm_page_count."]<BR>");
             $shm_bytes_written = shmop_write($this->shm_id, $shm_page_count, 0);
             if($shm_bytes_written != strlen($shm_page_count)) {
                 echo "writeMemoryNumber: Normal Couldn't write the entire length of data<BR>";
             }
          }
      }

      function close()
      {
	 if ($this->shm_id != null ) shmop_close($this->shm_id);
      }
}

class MemoryCounters
{
      var $counterNames;
      var $counterObjects;
      var $create_result_ok;
      var $debug_on;

      function MemoryCounters($no_create = false)
      {
         global $counterNames;
         $this->counterNames = $counterNames;
         $this->counterObjects = Array();

         if ($no_create == true) return;

         foreach( $this->counterNames as $name => $counter ) {

             $memory_size = 60;
	     $use_mode = HOURLY_MEM;

	     if (is_array($counter)) {
	         $t_counter = $counter[0];
	         $memory_size = $counter[1];
	         $use_mode = STANDARD_MEM;
             }
	     else $t_counter = $counter;

             echo("Creating shared memory instance of counter name ".$name. " value ".$t_counter."<br>");

             $t_memObj = new MemoryStore($t_counter, $use_mode, MEM_MODE_CREATE, $memory_size, true);
             //if ($name == "publish") $this->shm_id=0 ;
             if ($t_memObj->shm_id <= 0 ) {
                 echo("Warning: Shared memory id ".$t_counter." returned error (".$t_memObj->shm_id."). Possibly in use. <br>");
                 $this->create_result_ok = false;
                 return;
	     } else {
                 $this->counterObjects[$name] = $t_memObj;
	     }
             $this->create_result_ok = true;
         }
      }
      function DestroySubset()
      {
         // This should only really be called when there was a failure during create. A sub set of the memory
         // objects are assumed to have been created.
         //if ($this->counterObjects == null) return;

         foreach( $this->counterObjects as $sh_name => $t_memObject ) {
             echo("Deleting shared memory instance of counter name ".$sh_name. " value ".$t_memObject->mem_id.".");
             // Have to open it in read_only to get the id to delete it. Default size is 10.
             $t_shm_id = $t_memObject->shm_id;
	     if ($t_shm_id == 0) {
                 echo(" No shared memory existing for: " .$t_memObject->mem_id. "<br>");
             } else {
                 $t_res = shmop_delete($t_shm_id);
                 shmop_close($t_shm_id);
                 echo(" Delete result: " .$t_res. "<br>");
             }
         }
      }

      function CheckInstancesExist()
      {
         // For this to be called the constructor must be called which already sets up the array.
         global $counterNames;
         $this->counterNames = $counterNames;
         $this->counterObjects = Array();

         echo("<b>Checking if shared memory instances already exists.</b><br>");
         foreach( $this->counterNames as $name => $counter ) {
             //echo("Checking if shared memory instance of counter name ".$name. " value ".$counter." already exists.");
             // Have to open it in read_only to get the id to delete it. Default size is 10.
	     if (is_array($counter)) $t_counter = $counter[0];
	     else $t_counter = $counter;

             $t_shm_id = @shmop_open($t_counter, "a", 0, 0);
	     if ($t_shm_id == 0) {
                 echo(" No shared memory existing for (".$name.") " .$t_counter. "<br>");
             } else {
                 echo(" Shared memory ".$t_shm_id." exists for (".$name.") " .$t_counter. "<br>");
		 $shm_size = shmop_size($t_shm_id);
		 $shm_data = shmop_read($t_shm_id, 0, $shm_size);
                 echo(" Contents are ".$shm_size." bytes [".$shm_data."]<br>");
                 shmop_close($t_shm_id);
             }
         }
      }

      function DestroyInstances()
      {
         // For this to be called the constructor must be called which already sets up the array.
         global $counterNames;
         $this->counterNames = $counterNames;
         $this->counterObjects = Array();

         foreach( $this->counterNames as $name => $counter ) {
	     if (is_array($counter)) $t_counter = $counter[0];
	     else $t_counter = $counter;
             echo("Deleting shared memory instance of counter name ".$name. " value ".$t_counter.".");
             // Have to open it in read_only to get the id to delete it. Default size is 10.
             $t_shm_id = shmop_open($t_counter, "a", 0, 0);
	     if ($t_shm_id == 0) {
                 echo(" No shared memory existing for: " .$t_counter. "<br>");
             } else {
                 $t_res = shmop_delete($t_shm_id);
                 shmop_close($t_shm_id);
                 echo(" Delete result: " .$t_res. "<br>");
             }
         }
      }

}
// These are the shared memory counters. Do not change anything or add anything to this list
// on a live system until you first deactivate the shared memory through the configuration screen.
// Note: There is a small chance that these ids may clash with other programs running on the same server.
//       because these ids are supposed to be unique across the system. Ideally ftok() should be used.
//       but it was decided against it to avoid multiple file i/o calls for every request.
//       All the ids are in the 350nnn range.
    
$counterNames = Array( "Main" => 350300, "SpamRedirect" => 350301, "SpamBotBan" => 350302, "SpamMaxUri" => 350303, 
                       "contact_req" => 350304, "publish" => 350305, "publish_spammer" => 350306,
		       "newswire" => 350307, "archive" => 350308, "search" => 350311, "events" => 350312, 
		       "Contact_Send_Ok" => 350314, 
                       "global" => 350501, "index" => 350502, "article" => 350503, "comments" => 350504, 
                       "htmlcache" => 350505, 
                       "EditorLogin" => 350315, "LoginFail" => 350316, "POST_Req" => 350317, "POST_Req" => 350318,
                       "admin" => 350519, 
                       "ContactTimeStamp" => array(350400, 600), "ContactIpList" => array(350401, 600),
                       "PublishTimeStamp" => array(350402, 600), "PublishIpList" => array(350403, 600),
                       "HttpReferTimeStamp" => array(350404, 6000),
                       "HttpReferList" => array(350405, 12000),
                       "MemoryUsage" => array(350931, 4000), "MostPopular" => array(350934, 12000),
                       "ArticleDecay" => array(350932, 40),
                       "DbOpens" => 350936, "DbNoOpens" => 350937);

    // List not used as it is already in the one above
    $listNames = Array( "ContactTimeStamp" => array(350400, 600), "ContactIpList" => array(350401, 600) );

// General Shared Memory functions
function SharedMemoryUpdate($CounterName)
{
    global $system_config, $semaphore_id;
    global $counterNames;
    
    if ($system_config->memory_mgmt_installed != true) return;

    if ($system_config->memory_mgmt_activated != true) return;

    if ($CounterName == null) return;

    if (array_key_exists($CounterName, $counterNames)) {
        if (is_array($counterNames[$CounterName])) {
             $t_ptr = $counterNames[$CounterName];
             $memory_ptr = $t_ptr[0];
        }
	else $memory_ptr = $counterNames[$CounterName];
    } else {
        return;
    }

    // Add code to sem_get and then sem_acquire a semaphore
    $acq_res = sem_acquire($semaphore_id);
    if ($acq_res === FALSE && $system_config->user_error_reporting > 1) 
        logMessage("Error accquiring lock for semaphore id ".$semaphore_id." for semaphore key ".SEM_KEY_ID." for request ".$_SERVER['REQUEST_URI'], "sitelog.txt");

    $MemObjMemory = new MemoryStore($memory_ptr, HOURLY_MEM, MEM_MODE_WRITE);
    if ($MemObjMemory->shm_id == null || $MemObjMemory->shm_id === FALSE) {
        $rel_res = sem_release($semaphore_id);
        // Turn off shmem for the rest of this request and if editor logged in, generate log entry.
        $system_config->memory_mgmt_activated = false;
	$t_up = serverUpTime();
	if ($t_up != null) {
	    // If the uptime is less than when the memory was activated then we probably had a reboot.
            $t_msg = "Shared memory shmop() is generating errors. System appears to have been rebooted since memory was activated. Sharmed memory is being deactivated.";
	    if ($t_up < (time() - $system_config->memory_mgmt_activate_time)) {
                $system_config->memory_mgmt_activate_time = 0;
                $system_config->saveMemoryState();
                logMessage($t_msg, "sitelog.txt");
	        $editorStatusList = new PublishState();
                $editorStatusList->load();
                $editorStatusList->add("system", "post", time(), $t_msg);
                $editorStatusList->save();
	    } else {
                logMessage($t_msg, "sitelog.txt");
	    }
        } else {
        // Session is not started when Main called, but if the script is not index.php then it is probably an editor
        //if ($CounterName == "Main" && basename($_SERVER['PHP_SELF']) != "index.php") logMessage("Shared memory shmop() is generating errors. Need to deactivate and check system.","sitelog.txt");
            logMessage("Shared memory shmop() is generating errors. Need to deactivate and check system.","sitelog.txt");
        }
	return 0;
    }
    $page_count = $MemObjMemory->incMemoryNumber();
    $MemObjMemory->close();
    $rel_res = sem_release($semaphore_id);

    if ($CounterName == "Main") {
        if (($page_count % 10 ) == 0) {
            list($MemoryUsageId,$MemTrkSize) = GetSharedMemoryId("MemoryUsage",true);
            $max_grph_pts = 38;

	    // Windows does not support this function.
	    if (strcasecmp(substr(PHP_OS,0,3), "WIN") == 0) {
               // Do nothing.
	    } else {
               if (phpversion() > 4.3 ) {
                   $peak_value = floor(memory_get_usage()/1024);
                   $tmp_array = MemoryTracker($MemoryUsageId, $max_grph_pts, $peak_value, $MemTrkSize);
               }
           }
        }
    }

    return $page_count;
}

function SharedMemoryRead($CounterName, $read_all_hours = false)
{
    global $system_config;
    global $counterNames;
    
    if ($system_config->memory_mgmt_installed != true) return null;
    if ($system_config->memory_mgmt_activated != true) return null;

    if ($CounterName == null) return null;


    if (array_key_exists($CounterName, $counterNames)) {
        if (is_array($counterNames[$CounterName])) {
            $t_ptr = $counterNames[$CounterName];
            $memory_ptr = $t_ptr[0];
        } else {
            $memory_ptr = $counterNames[$CounterName];
        }
    } else {
        return null;
    }

    $MemObjMemory = new MemoryStore($memory_ptr, HOURLY_MEM, MEM_MODE_READ);
    $page_count = $MemObjMemory->readMemoryNumber($read_all_hours);
    $MemObjMemory->close();

    if (is_array($page_count)) return $page_count;

    if ($page_count == null OR $page_count == "") $page_count = 0;

    // echo("Page Count: ".$page_count. "<BR>");
    return $page_count;
}

// General Shared Memory Id 
function GetSharedMemoryId($CounterName, $GetSizeToo=false)
{
    global $system_config;
    global $counterNames;
    
    if ($system_config->memory_mgmt_installed != true) return null;
    if ($system_config->memory_mgmt_activated != true) return null;

    if ($CounterName == null) return null;

    // For the moment it defaults to: 24*10 + 5
    $max_size = 245;
    if (array_key_exists($CounterName, $counterNames)) {
        if (is_array($counterNames[$CounterName])) {
             $t_ptr = $counterNames[$CounterName];
             $memory_ptr = $t_ptr[0];
             $max_size   = $t_ptr[1];
        }
	else $memory_ptr = $counterNames[$CounterName];

	if ($GetSizeToo == true) 
	    return array($memory_ptr,$max_size);
	else 
	    return $memory_ptr;
    } else {
        return null;
    }

}

function SharedMemoryStateChange()
{
    global $system_config;
    
    if ($system_config->memory_mgmt_installed != true) return;

    if ($system_config->memory_mgmt_activated == true) {
        // Assumption is that it is just activated so create memory.
        // Note: Actual state does get saved to database until returning from this function.
        echo("Creating ...<BR>");
        $instance = new MemoryCounters();
        if ($instance->create_result_ok == true) return true;

	// Otherwise there was a failure and we need to delete them again.
        echo("Aborting Shared Memory Activation. Deleting previously created memory blocks ...<BR>");
        $instance->DestroySubSet();
        return false;

    } else {
        // Assumption is that it is just de-activated so delete memory.
        echo("Deleting ...<BR>");
        $instance = new MemoryCounters(true);
        $instance->DestroyInstances();
    } 
    return true;
}
// This will check if the shared memory exists for the pre-defined set in the array. It should be called
// before activation to make sure it is not in use by some other program.
function SharedMemoryExistCheck()
{
    global $system_config;
    
    if ($system_config->memory_mgmt_installed != true) return;

    if ($system_config->memory_mgmt_activated == false) {
        // Useful check.
        $instance = new MemoryCounters(true);
        $instance->CheckInstancesExist();
    } else {
        // Assumption is that it is just de-activated so delete memory.
        echo("Shared Memory Already Activated. No need for check.<BR>");
    } 
}
// Reset Shared Memory counters.
function SharedMemoryReset()
{
    global $system_config, $semaphore_id;
    global $counterNames;
    
    if ($system_config->memory_mgmt_installed != true) return;
    if ($system_config->memory_mgmt_activated != true) return;


    // Add code to sem_get and then sem_acquire a semaphore
    $ret_str = "";
    // Beware: Avoid use of echo inside semaphore usage.
    $acq_res = sem_acquire($semaphore_id);
    if ($acq_res === FALSE && $system_config->user_error_reporting > 1) 
        logMessage("Error accquiring lock for semaphore id ".$semaphore_id." for semaphore key ".SEM_KEY_ID." for request ".$_SERVER['REQUEST_URI'], "sitelog.txt");

    foreach( $counterNames as $name => $counter ) {

	$use_mode = HOURLY_MEM;

	if (is_array($counter)) {
	    $t_counter = $counter[0];
	    $use_mode = STANDARD_MEM;
        }
	else $t_counter = $counter;

        $t_memObj = new MemoryStore($t_counter, $use_mode, MEM_MODE_WRITE);
        if ($t_memObj->shm_id <= 0 ) {
            $ret_str .= "Warning: Shared memory id ".$t_counter." returned error (".$t_memObj->shm_id."). Unable to open memory segment. <br>";
            $ret_code = false;
            continue;
	}
        $t_memObj->initMemoryNumber();
        $t_memObj->close();
    }
    $rel_res = sem_release($semaphore_id);
    return $ret_str;

}
// This function is called to record the current memory usage. It is a ring buffer essentially.
// It returns an array of counters stored so far.
function MemoryTracker($MemoryUsageId, $MaxPts, $UsedMemory, $MemSize, $DoUpdate = true)
{
    global $semaphore_id;
    //$MemoryUsageId = GetSharedMemoryId("MemoryUsage");
    if ($MaxPts < 1) $MaxPts = 25;

    // Be really careful with these. Make sure they are not nested otherwise it will block
    $acq_res = sem_acquire($semaphore_id);
    if ($acq_res === FALSE && $system_config->user_error_reporting > 1) 
        logMessage("Error accquiring lock for semaphore id ".$semaphore_id." for semaphore key ".SEM_KEY_ID." for request ".$_SERVER['REQUEST_URI'], "sitelog.txt");

    if ($DoUpdate == true) {
        $PkMem = new MemoryStore($MemoryUsageId, STANDARD_MEM, MEM_MODE_WRITE, $MemSize);
    } else {
        $PkMem = new MemoryStore($MemoryUsageId, STANDARD_MEM, MEM_MODE_READ, $MemSize);
    }

    $last_list = $PkMem->readMemory();
    $t_debug = false ;
    // $t_debug = true ;
    if ($t_debug == true) {
        echo("Update Flag : [".$DoUpdate . "] MaxPts: ".$MaxPts." MemSize ".$MemSize."<BR>");
        echo("Prev Max Mem: [".$last_list . "]<BR>");
        echo("This Max Mem: [".$UsedMemory . "]<BR>");
    }
    $tmp_array = explode(" ",$last_list);

    $list_count = count($tmp_array);
    if ($t_debug == true) echo("No elements: ".$list_count."<BR>");

    if ($DoUpdate == true) {
        array_push($tmp_array, $UsedMemory);
           
        if ($list_count > $MaxPts ) array_shift($tmp_array);
        $new_peak = implode(" ",$tmp_array);
        if ($t_debug == true) echo("Will write list: ".$new_peak."<BR>");
        $PkMem->writeMemory($new_peak);
    }
    $PkMem->close();
    $rel_res = sem_release($semaphore_id);

    return $tmp_array;
}
//  To Do. Add in functon that counts every say 10x counts, and then gets memory usage or other info
//  Add in a per hour counter tracker.

// This function is used to implement the determination of the most popular article clicked on.
//
function MemoryFrequentTracker($MemoryUsageId, $ArticleId, $ReadDataOnly = false, $ResetData = false, $MaxSize=0)
{
    global $semaphore_id;

    $t_debug = false ;
    // $t_debug = true ;

    // i.e n*14 + 6 + 1 <= 600  .. MaxPts is only going to be used during a potential write op and in those
    // cases, the proper value should always be passed but default it to 40 just in case... which is 600 bytes
    if ($MaxSize > 0) {
        $MaxPts = floor($MaxSize / 15);
        if ($t_debug == true) echo("Max Entries is: ".$MaxPts . "<BR>");
    } else {
        $MaxPts = 40;
    }

    // Be really careful with these. Make sure they are not nested otherwise it will block
    $acq_res = sem_acquire($semaphore_id);
    if ($acq_res === FALSE && $system_config->user_error_reporting > 1) 
        logMessage("Error accquiring lock for semaphore id ".$semaphore_id." for semaphore key ".SEM_KEY_ID." for request ".$_SERVER['REQUEST_URI'], "sitelog.txt");

    // With this block of code for every 10 hits, we decay all counters except the current one by one.
    // Otherwise the most populat story would be for all time.
    $DecayCount = false;
    if ($ReadDataOnly == false) {
        $DecayCount = false;
        $t_ShmemId = GetSharedMemoryId("ArticleDecay");
        if ($t_ShmemId != null) {
            $MemObjMemory = new MemoryStore($t_ShmemId, STANDARD_MEM, MEM_MODE_WRITE, 40);
            if ($MemObjMemory->shm_id != null && $MemObjMemory->shm_id != FALSE) {
                $page_count = $MemObjMemory->incMemoryNumber();
                $MemObjMemory->close();
                // If can't access just continue anyhow.
                if ( ($page_count % 1024 ) == 0 ) $DecayCount = true;
            }
        }
    }

    if ($ReadDataOnly == false) {
        $PkMem = new MemoryStore($MemoryUsageId, STANDARD_MEM, MEM_MODE_WRITE);
    } else {
        $PkMem = new MemoryStore($MemoryUsageId, STANDARD_MEM, MEM_MODE_READ);
    }
    $PkMem->setSize();

    if ($ResetData == true) {
        $PkMem->writeMemory("");
        $PkMem->close();
        $rel_res = sem_release($semaphore_id);
        return null;
    }

    // Article Id and Counts are stored as pairs like article_id:count article_id:count article_id:count
    // Sizing is to give 6 digits to article and 6 to number. One set = 6+6+1+1 = 14. Then 140 is 10 and 896 = 64
    $ArticleList = $PkMem->readMemory();

    if ($t_debug == true) echo("List Data is: [".$ArticleList . "] ");
    if ($t_debug == true) echo("Input Article [".$ArticleId . "] ");

    $FoundIt = false;
    $ArticleArray = array();
    if (strlen($ArticleList) == 0) {
        $ArticleCount = 0;
    } else {
        $tmp_array = explode(" ",$ArticleList);
        $ArticleCount = count($tmp_array);
    }

    // If article id not found add it, otherwise increase the count.
    if ($t_debug == true) echo("No Elements: ".$ArticleCount."<BR>");

    if ($ArticleCount > 0) {
        foreach ($tmp_array as $element_pair) {
            list($article, $a_count) = explode(":", $element_pair);
            if ($t_debug == true) echo("Cmp ". (int)$article ." to ". $ArticleId."<BR>");
	    if ((int)$article == $ArticleId) {
	        $a_count++;
	        $FoundIt = true;
	    }

            if ($t_debug == true) echo("Array: ".$article." => ". $a_count."<BR>");
            $ArticleArray[$article] = $a_count;
        }
        if ($DecayCount == true) {
            foreach ($ArticleArray as $t_key => $t_value) {
    	        if ((int)$t_key == $ArticleId) continue;
    	        if ((int)$t_value > 0) $ArticleArray[$t_key]--;
            }
        }
    }

    if ($ReadDataOnly == false) {
        if ($FoundIt == false) {
            // Sort array by count and then bump off the last element if it has max entries and add this one.
            if ($t_debug == true) foreach($ArticleArray as $k => $v) { echo("PreSort ".$k. "=>".$v."<BR>"); }
            arsort($ArticleArray, SORT_NUMERIC);
            if ($t_debug == true) foreach($ArticleArray as $k => $v) { echo("PstSort ".$k. "=>".$v."<BR>"); }
           
            if (count($ArticleArray) > $MaxPts ) array_pop($ArticleArray);
            // array_push($ArticleArray, $ArticleId => 1 );
            $ArticleArray[$ArticleId] = 1;
        }

        // Now write the array back but expand it again.
        $mem_data = "";
        foreach ($ArticleArray as $article_id => $a_count ) {
            $mem_data .= $article_id.":".$a_count." ";
        }
	// Remove the trailing space
        $mem_data = trim($mem_data);

        if (strlen($mem_data) < $MaxSize )
            $PkMem->writeMemory($mem_data);
    }

    $PkMem->close();
    $rel_res = sem_release($semaphore_id);

    //if ($ReadDataOnly == true) echo("No Elements: ".$ArticleCount."<BR>");
    //if ($ReadDataOnly == false) echo("Elements: ".$ArticleList."<BR>");

    return $ArticleArray;
}
// This function is a generic wrapper to track an article count.
//
function UpdateArticleTracker($Story_Id)
{
       list($ShmemId, $MaxSize) = GetSharedMemoryId("MostPopular",true);
       if ($ShmemId == null) return;

       $DummyArray = MemoryFrequentTracker($ShmemId, $Story_Id, false, false, $MaxSize);
}
function UpdateMemoryPublishUpdate()
{
       $current_time = time();
       $current_ip   = $_SERVER['REMOTE_ADDR'];

       // Store IP and time in a dual memory lifo buffer
       $ShmemTimeId = GetSharedMemoryId("PublishTimeStamp");
       $ShmemIpId   = GetSharedMemoryId("PublishIpList");

       // Beware hard-coding 600. This should be more robust.
       sem_acquire($semaphore_id);
       $TimestampMem = new MemoryStore($ShmemTimeId, STANDARD_MEM, MEM_MODE_WRITE, 600);
       $IPAddressMem = new MemoryStore($ShmemIpId, STANDARD_MEM, MEM_MODE_WRITE, 600);

       $TimeList = $TimestampMem->readMemory();
       $IpList = $IPAddressMem->readMemory();

       $TimestampMem->writeMemoryAsList($TimeList, $current_time, 10);
       $IPAddressMem->writeMemoryAsList($IpList, $current_ip, 10);

       $TimestampMem->close();
       $IPAddressMem->close();
       sem_release($semaphore_id);
}

function UpdateMemoryHttpReferer()
{
       global $system_config;
       global $semaphore_id;
       // $current_time = time();
       if (!isset($_SERVER['HTTP_REFERER'])) return;

       $http_refer = $_SERVER['HTTP_REFERER'];
       //if (stripos($system_config->site_url, $http_refer) === FALSE )
       if (strpos($system_config->site_url, $http_refer) === FALSE )
       {
           $http_refer = substr($http_refer,0,120);
           // Store IP and time in a dual memory lifo buffer. Dont store it if the address is this site
           //$ShmemTimeId = GetSharedMemoryId("PublishTimeStamp");
           $ShmemRefrId = GetSharedMemoryId("HttpReferList");
           if ($ShmemRefrId == null) return;
    
           // Beware hard-coding 600. This should be more robust.
           sem_acquire($semaphore_id);
           //$TimestampMem = new MemoryStore($ShmemTimeId, STANDARD_MEM, MEM_MODE_WRITE, 600);
           $ReferAddressMem = new MemoryStore($ShmemRefrId, STANDARD_MEM, MEM_MODE_WRITE, 12000);
           //$TimeList = $TimestampMem->readMemory();
           $ReferList = $ReferAddressMem->readMemory();
    
	   //echo "writing ".$http_refer . " <BR>";
           //$TimestampMem->writeMemoryAsList($TimeList, $current_time, 10);
           $ReferAddressMem->writeMemoryAsList($ReferList, $http_refer, 100);
    
           //$TimestampMem->close();
           $ReferAddressMem->close();
           sem_release($semaphore_id);
       }
}


// Function to see if it can read uptime. Returns null if it can't
// Will also check the Db Uptime as a bad substitute. Value returned is number of uptime seconds.
function serverUpTime($TryServerOnly=false)
{
       $t_safe = ini_get('safe_mode');
       if (($t_safe == null or $t_safe == 0) && strcasecmp(substr(PHP_OS,0,3), "WIN") != 0) {
           $t_handle = fopen("/proc/uptime","r");
	   if ($t_handle != null) {
               $t_up_data = fgets($t_handle,1024);
               fclose($t_handle);
	       if ($t_up_data != false) {
                   list($t_up, $t_dum) = preg_split("/ /",$t_up_data);
                   return $t_up;
               }
	   }
       }

       if ($TryServerOnly == true) return null;

       global $dbconn;
       // Zero means no db cache
       $result = sql_query("SHOW STATUS LIKE 'uptime'", $dbconn, 0);
       checkForError($result);

       $db_uptime = array();
       $db_uptime_name = "";
       $db_uptime_value = "";

       if(sql_num_rows( $result ) > 0)
       {
           list($db_uptime_name, $db_uptime_value) = sql_fetch_row($result, $dbconn);
       }

       if ($db_uptime_value != "") return $db_uptime_value;

       return null;
}
?>
