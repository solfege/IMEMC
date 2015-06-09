<?
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

require_once('displayobject.inc');
require_once('objects/bannedip.inc');
require_once('objects/formvalidate.inc');
require_once('objects/publishmonitor.inc');
require_once('objects/memorymgmt.inc');


class contactDisplay extends displayObject
{

   function contactDisplay ($obj_id)
   {
      $this->displayObject($obj_id);
   }
   
   /**
   * Clears header field to avoid injection
   * http://www.anders.com/projects/sysadmin/formPostHijacking/
   * http://www.davidseah.com/archives/2005/09/01/wp-contact-form-spam-attack/
   */
   function preprocessHeaderField($value)
   {
      //Remove line feeds
      $ret = str_replace("\r", "", $value);
      $ret = str_replace("\n", "", $ret);
      // Remove injected headers
      $find = array("/bcc\:/i",
                    "/Content\-Type\:/i",
                    "/Mime\-Type\:/i",
                    "/cc\:/i",
                    "/to\:/i");
      $ret = preg_replace($find,
                          "**bogus header removed**",
                          $ret);
      return $ret;
   }

   function processRequest()
   {
      global $system_config, $OSCAILT_SCRIPT;

      parent::processRequest();

      SharedMemoryUpdate("contact_req");
      //first check the blocked ips from the database
      $ipBanList = new BannedIPList();
      $ipBanList->load();
      $ip = cleanseIP($_SERVER['REMOTE_ADDR']);
      if($ip === false or $ipBanList->isBanned($ip))
      {
         if($ipBanList->isExpired($ip) == false)
         {
            $this->writeError($ipBanList->getBannedMessage($ip), false);
            $this->result = "publishing_banned";
            if ($system_config->security_recording_level >= 1)
                logMessage("Contact form (".$this->getDisplayOption('title') .") blocked for banned IP address ".$ip , "sitelog.txt");
	    // Add delay to stop excess refresh.
   	    sleep(2);
            return false;
         }
      }

      $successtext = $this->getDisplayOption("successtext");
      if(isset($_REQUEST['send_message']))
      {
         $formValidationObj = new formValidator(2);
         if(strlen(trim($_REQUEST['name']))==0)
         {
            $this->writeError("You must specify your name!");
         }
         else if(strlen(trim($_REQUEST['email']))==0)
         {
            $this->writeError("You must specify your email address!");
         }
         elseif(!isValidEmailAddress(trim($_REQUEST['email'])))
         {
            $this->writeError("You must specify a valid email address!");
         }
         else if($this->getDisplayOption("hidesubjectfield") != 'true' && strlen(trim($_REQUEST['subject']))==0)
         {
            $this->writeError("You must specify the subject!");
         }
         else if(strlen(trim($_REQUEST['message']))==0)
         {
            $this->writeError("You must include a message!");
         }
         else if(!isset($_REQUEST['tickbox']) && $this->getDisplayOption("spambotcheck") == 'true')
         {
            $_SERVER['REQUEST_METHOD'] = "GET";
            $this->writeError("You must tick the checkbox!");
         }
         else
         {
            if($formValidationObj->validateFormData() != true)
            {
               $formValidationObj->reportInvalidation( $this->obj_id);
               echo "<div class='user-message'>$successtext</div>\n";
	       return true;
            }

            $invalid_rsp = false;
            $invalid_msg = "";
            // $ength_url = strlen($system_config->site_url);

            if($this->checkIfPostBefore() == true)
            {
                $invalid_rsp = true;
                $invalid_msg .= "Possible Spam. " . $_REQUEST['name'] . " posted before. Subject was:".$_REQUEST['subject'];
		// So ban them for awhile (2hrs) and generate a notice
		$ban_end = time() + (2 * 60 * 60);
                $r = "Contact form possible spam attack. Name used: ".$_REQUEST['name'].". " . $ip . ". IP banned for 2 hrs";
                $ipBanList->banIP($ip, $ban_end, $r);
                $logMsg = "system:\tban:\tIP:\t" . $ip . ":\t " .$r;
                logMessage($logMsg, "actionlog.txt");
                // To notify by email will require some changes to that code.
 
            }

            if(strlen($_REQUEST['name']) > 60)
            {
                $invalid_rsp = true;
                $invalid_msg .= "Name Field length=" . strlen($_REQUEST['name']) . " ";
            }
            if(strlen($_REQUEST['subject']) > 200)
            {
                $invalid_rsp = true;
                $invalid_msg .= "Subject Field length=" . strlen($_REQUEST['subject']) . " ";
            }

            if(!isset($_SERVER['HTTP_USER_AGENT']))
            {
                $invalid_rsp = true;
                $invalid_msg .= "HTTP_USER_AGENT NOT set. ";
            }

            // If browser security settings are high this will not be set.
            if(!isset($_SERVER['HTTP_REFERER']))
            {
                if (!isset($_SERVER['REQUEST_METHOD'])) $invalid_rsp = true;
                elseif ($_SERVER['REQUEST_METHOD'] != "POST") $invalid_rsp = true;

                if ($invalid_rsp == true) $invalid_msg .= "No referer set. ";
            }
            $subject = (isset($_REQUEST['subject'])) ? $_REQUEST['subject'] : "";
            $name = (isset($_REQUEST['name'])) ? htmlspecialchars($_REQUEST['name']) : "";
            $email = (isset($_REQUEST['email'])) ? htmlspecialchars($_REQUEST['email']) : "";
            $subject = str_replace("\n", " ", $subject);
            $subject = str_replace("\r", " ", $subject);
            $name = str_replace("\n", " ", $name);
            $name = str_replace("\r", " ", $name);
            $message = (isset($_REQUEST['message'])) ? htmlspecialchars($_REQUEST['message']) : "";

            $email = $this->preprocessHeaderField($email);
            $subject = $this->preprocessHeaderField($subject);

	    // Really means for reported posts. Also need to limit the size of the message too.
            $mandatory_list = trim($this->getDisplayOption("spammandatorywords"));
            if( strlen($mandatory_list) > 0)
	    {
		// Most of the time there will be at most one word.
                $word_array = explode(" ",$mandatory_list);
                $word_count = count($word_array);
		$any_found = false;
                for ($i = 0; $i < $word_count; $i++) {
                     if( strpos($message, $word_array[$i]) === false) continue;

		     $any_found = true;
		     break;
		}
                if( $any_found == false) {
		    $invalid_rsp = true;
                    $invalid_msg .= "Reason(50) ";
		}
	    }

            if ($invalid_rsp == true)
	    {
               // This will build a nice message for reporting and log it to the spam log
               if($this->getDisplayOption("logspamcheck") == 'true')
	       {
                   $this->handleInvalidMessage($invalid_msg, $subject, $name, $email, $message);
               }
               $success = TRUE;
            }
	    else
	    {
               if($this->getDisplayOption("logemails") == 'true')
	       {
                  # These messages are formatted and written to the reported_post.txt file without updating
                  # the functionality that displays all these posts in the viewsitelog

		  // Need to add the angle bracks as &gt; etc etc
                  $msg .= "Subject: " .$subject . "\nEmail: " . "\"" . $name . "\" " . $email ;
                  if(isset($_REQUEST['ref_url']) )
		  {
                     $msg .= "\nRef_URL: " . $_REQUEST['ref_url'];
                  }
                  $msg .= "\nMsgText: " . $message;
                  $msg = $this->formatMessageForLogs($msg);

                  logEmail($msg, "reported_post.txt");
               }

               // Was passing $_REQUEST['email'] as input which was incorrect really. Same for name & message
               $success = $this->sendContactMessage($name,$email,$subject,$message);

            }
            if(!$success)
            {
               $error_message = "failed to send message from contact form.\nName: [$name]\nEmail: [$email]\nSubject: [$subject]\n\n$message\n\n";

               $this->writeError("Delivery of email failed - system error", false);
            }
            else
            {
               echo "<div class='user-message'>$successtext</div>\n";
            }
         }
      }
      else $this->display();

      return true;
   }

   function display()
   {
      $this->writeDisplayHeader();
      $this->writeContactIntro();
      $this->writeContactForm();
      $this->writeDisplayFooter();
   }

   function writeContactIntro()
   {
      $title = $this->getDisplayOption("title");
      $introtext = $this->getDisplayOption("introtext");
      echo "<big><b>$title</b></big><p>$introtext</p>";
   }

   function writeContactForm()
   {
      global $system_config, $OSCAILT_SCRIPT;
      $finalchecktext = $this->getDisplayOption("finalchecktext");
      $nametitle = $this->getDisplayOption("nametitle");
      $contentstitle = $this->getDisplayOption("contentstitle");
      $subjecttitle = $this->getDisplayOption("subjecttitle");
      $fromtitle = $this->getDisplayOption("fromtitle");
      $buttontext = $this->getDisplayOption("buttontext");
      $formheadtext = $this->getDisplayOption("formheadtext");
      $name = (isset($_REQUEST['name'])) ? htmlspecialchars($_REQUEST['name']) : "";
      $subject = (isset($_REQUEST['subject'])) ? htmlspecialchars($_REQUEST['subject']) : "";
      $email = (isset($_REQUEST['email'])) ? htmlspecialchars($_REQUEST['email']) : "";
      $message = (isset($_REQUEST['message'])) ? htmlspecialchars($_REQUEST['message']) : "";

      $formValidationData = new formValidator(2);
      $hiddenData = $formValidationData->getHiddenData();
      ?>
      <form name="messaging_frm" action="<?=$OSCAILT_SCRIPT?>" method="post">
      <input type="hidden" name="obj_id" value="<?=$this->obj_id?>">
      <?
      // If it is reported post, then keep the url separate as it is embedded in the incoming message
      if (strpos($message, "http") !== false)
      {
	  $ref_url = substr($message, strpos($message, "http"));
          if (strpos($message, "\n") !== false) $ref_url = substr($ref_url, 0, strpos($ref_url, "\n"));
          ?>
          <input type="hidden" name="ref_url" value="<?=$ref_url?>">
          <?
      }
      ?>
      <?=$hiddenData?>
      <table width="100%" class="publish" border=0>
      <tr>
         <td>
         <table width="100%" cellspacing=0 cellpadding=3 border=0>
         <tr class=publish>
            <th class=publish colspan=2>
            <b> <?=$formheadtext?></b>
            </th>
         </tr>
         <tr class=publish>
            <td class="publish-title" align=right><b><?=$nametitle?></b>:</td><td class=publish><input type=text name="name" value="<?=$name?>" size="30" maxlength="60"></td>
         </tr>
         <tr class=publish>
            <td class=publish align=right><b><?=$fromtitle?></b>:</td><td class=publish><input type=text name="email" value="<?=$email?>" size="30" maxlength="80"></td>
         </tr>
         <?
         if($this->getDisplayOption("hidesubjectfield") != 'true')
         {
         ?>
            <tr class=publish>
               <td class="publish-title"  align=right><b><?=$subjecttitle?></b>:</td><td class=publish><input type=text name="subject" value="<?=$subject?>" size="50" maxlength="100"></td>
            </tr>
         <?
         }
         ?>
         <tr class=publish>
            <td class="publish-title"  align=right valign=top><b><?=$contentstitle?></b>:</td>
            <td class=publish><textarea name="message" rows="20" cols="50"><?=$message?></textarea></td>
         </tr>
         <?
         if($this->getDisplayOption("spambotcheck") == 'true')
	 {
         ?>
         <tr class=publish>
            <td class="publish-title"  align=right valign=top><b><?=$finalchecktext?></b></td>
            <td class=publish><input type="checkbox" name="tickbox" value="off" onClick="updateValStat()"></td>
         </tr>
         <?
         }


         ?>
         <tr class=publish>
            <td class=publish colspan=2 align=center><input type=submit name="send_message" value="<?=$buttontext?>"></td>
         </tr>
         </table>
         </td>
      </tr>
      </table>
      </form>
      <script language="JavaScript">
      function updateValStat() {
      if(document.messaging_frm.tickbox.checked) document.messaging_frm.tickbox.value = "Go";
      else document.messaging_frm.tickbox.value = "Off";
      }
      </script>
      
      <br>&nbsp;
      <?
   }

   function sendContactMessage($name,$email,$subject,$message)
   {
      global $system_config, $editor_session;
      $emailto = $this->getDisplayOption("emailto");
      $emailfrom = $this->getDisplayOption("emailfrom");
      $emailreply = $this->getDisplayOption("emailreply");
      $subject .= $this->getDisplayOption("subjecttext");

      if($emailto == "") $emailto = $system_config->contact_email_address;
      if($this->getDisplayOption("editorprivatemail") == 'true')
      {
         $editor_emails = $this->getEditorEmailArray();
         $emailto = implode(", ", $editor_emails);
      }
      if($emailto == "") $emailto = $system_config->contact_email_address;
      if($emailfrom == "") $emailfrom = $system_config->notification_from_email_address;
      if($emailreply == "") $emailreply = $emailto;
      if($subject == "") $subject = "<not specified>";
      $time_msg =  strftime("%H:%M:%S %a", (time()+$system_config->timezone_offset));
      $message = "Contact Form Submission"."\r\n"."From: ".$name." ( mailto:".$email." )"."\r\n\r\n".$message . "\r\nEnd of Msg. Sent at " . $time_msg . "\r\n";
      //op_r(array($emailto, $subject, $message, "From: ".$emailfrom."\r\n"."Reply-To: ".$email.", ".$emailreply."\r\n"."X-Mailer: $system_config->software_name/$system_config->software_version PHP/" . phpversion()));
      //$success = false;
      //$success = mail("chekov@indymedia.ie", $subject, $message, "From: oscailt@indymedia.ie");
      $success =   mail($emailto, $subject, $message, "From: ".$emailfrom."\r\n"."Reply-To: ".$email.",".$emailreply."\r\n"."X-Mailer: $system_config->software_name/$system_config->software_version PHP/" . phpversion());

      if($system_config->publish_monitor_enabled)
      {
          $nm = $name;
	  if($editor_session->isSessionOpen())
          {
              $nm = $name . " (" .$editor_session->editor->editor_name." <small class='error'>editor</small>)";
          }
          $contact_str = "$nm sent email from contact from address: ".$email. " Subject: ". $subject;
          $monitorList = new PublishMonitor();
          $monitorList->load();
          $monitorList->add($contact_str);
          $monitorList->save();
      }

      return $success;
   }

   function handleInvalidMessage($invalid_txt, $subject, $name, $email, $message)
   {
      $msg = "Reasons: Likely to be spam as:" . $invalid_txt;
      if (isset($_SERVER['REQUEST_METHOD']))
      {
          $msg .= " REQUEST_METHOD: " . $_SERVER['REQUEST_METHOD'];
      }
      if (isset($_SERVER['REQUEST_URI']))
      {
          $msg .= " REQUEST_URI: " . $_SERVER['REQUEST_URI'];
      }
      if (isset($_SERVER['REDIRECT_URL']))
      {
          $msg .= " REDIRECT_QUERY: " . $_SERVER['REDIRECT_URL'];
      }
      // We may want to change this to show the actual form fields filled out.
      $msg .= "\nForm fields are:\nSubject: " . $subject . "\nEmail: " . "\"" . $name . "\" " . $email ;
      $msg .= "\nMsgText: " . $message;
      $msg = $this->formatMessageForLogs($msg);

      logSpammerMessage($msg, "spamlog_contact.txt", false);
   }

   function formatMessageForLogs($message_so_far)
   {
      global $system_config;
      # This routine puts a basic header and footer to make a basic format that is read by the view logs
      # Do not change the format of the message written here without updating the functionality that displays 
      # all these posts in the viewsitelog.
      # It puts a Start_Post and End_Post at the start and end.

      $ref_number = time();
      $msg = "--Start_Post-" . $ref_number ."--Time_Sent:". date("D, d M Y H:i:s",(time()+$system_config->timezone_offset))."\n"; 
      $msg .= $message_so_far . "\n--End_Post-" . $ref_number ."--\n";
      return $msg;

   }

   function writeError($error, $redraw = true)
   {
      $this->writeContactIntro();
      ?><div class='user-message'><center><span class="error"><B>Error: </B><?=$error?></span></center></div><?
      if($redraw) $this->writeContactForm();
   }

   function getEditorEmailArray()
   {
      require_once "objects/editor.inc";
      $emails = array();
      $editorList = new EditorList();
      foreach($editorList as $editor)
      {
         if(isValidEmailAddress($editor->editor_email))
         {
            $emails[] = $editor->editor_email;
         }
      }
      return $emails;
   }

   function checkIfPostBefore()
   {
       global $semaphore_id;
       global $system_config;
    
       if ($system_config->memory_mgmt_installed != true) return false;

       if ($system_config->memory_mgmt_activated != true) return false;

       $tolerance = 10;
       // Store access count.
       SharedMemoryUpdate("Contact_Send_Ok");

       $current_time = time();
       $current_ip   = $_SERVER['REMOTE_ADDR'];

       // Store IP and time in a dual memory lifo buffer
       $ShmemTiId = GetSharedMemoryId("ContactTimeStamp");
       $ShmemIpId = GetSharedMemoryId("ContactIpList");

       // Beware hard-coding 600. This should be more robust.
       sem_acquire($semaphore_id);
       $TimestampMem = new MemoryStore($ShmemTiId, STANDARD_MEM, MEM_MODE_WRITE, 600);
       $IPAddressMem = new MemoryStore($ShmemIpId, STANDARD_MEM, MEM_MODE_WRITE, 600);
       $TimeList = $TimestampMem->readMemory();
       $IpList = $IPAddressMem->readMemory();

       $TimestampMem->writeMemoryAsList($TimeList, $current_time, 10);
       $IPAddressMem->writeMemoryAsList($IpList, $current_ip, 10);
       $TimestampMem->close();
       $IPAddressMem->close();
       sem_release($semaphore_id);

       $TimeArray = explode(" ",$TimeList);
       $IpArray = explode(" ",$IpList);
       if (count($TimeArray) == 0 ) return false;
      
       $debug_this = false;
       $rating = 0;
       // Work backwards
       $listTotal = count($IpArray);
       for($i=0; $i < $listTotal;$i++)
       {
           $curIndex = ($listTotal -($i+1));
           if ($debug_this == true) echo("IP match ".$current_ip . " to ".$IpArray[$curIndex] ."<BR>");
           if ( $current_ip == $IpArray[$curIndex]) {
               // Now see if time difference is say 20 secs or less.
               if ($debug_this == true) echo("time ".$current_time . " to ".$TimeArray[$curIndex] ."<BR>");
               if ($i == 0 ) $last_time = $current_time;
	       else $last_time = $TimeArray[($curIndex+1)];

               if ($debug_this == true) echo("time diff ".($last_time - $TimeArray[$curIndex]) ."<BR>");
               if (($last_time - $TimeArray[$curIndex]) < $tolerance ) {
                   if ($debug_this == true) echo("Time matches <BR>");
		   $rating++;
                   if ($rating >= 2 && $debug_this == true) echo("returning true <br>");
                   if ($rating >= 2) return true; 
               }
           } else {
	       if ($rating > 0 ) $rating--;
           }
      }
      if ($debug_this == true) echo("returning false <BR>");
      return false;

   }
}
?>
