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


/************************************************************************/
/* Base class for all indy objects - just has error handling functions  */
/************************************************************************/
class indyObject
{

   var $userMessages = array();

    /************************************************************************/
    /* errors and user messages */
    /************************************************************************/
    function setUserMessages ( $arr )
    {
        foreach ( $arr as $msg )
        {
            $this->userMessages[] = $msg;
        }
    }

   function setSQLError($result, $code = "cnt")
   {
      $this->setUserMessage(array(INDY_ERROR, "database error : ".mysql_errno()." - ".mysql_error(), $code));
   }

   function resetUserMessages()
   {
      $this->userMessages = array();
   }

    function setUserMessage( $arr )
    {
        $this->userMessages[] = $arr;
    }

    function getUserMessages()
    {
        return $this->userMessages;
    }

   function writeUserMessageBox()
   {
      global $system_config;
      if (count( $this->userMessages ) == 0 ) return;
      $msgs = array_reverse($this->userMessages);
      $msgs_to_print = array();
      $msgs_to_log = array();
      foreach( $msgs as $msg )
      {
         if( $msg[0] == USER_ERROR) $msgs_to_print[] = $msg[1];
         if( $msg[0] == USER_WARNING && $system_config->user_error_reporting >= 1) $msgs_to_print[] = $msg[1];
         if( $msg[0] == USER_MSG && $system_config->user_error_reporting > 1) $msgs_to_print[] = $msg[1];
         if( $msg[0] > $system_config->debug_level ) continue;
         elseif($msg[0] > USER_MSG && $system_config->user_error_reporting > 1)
         {
            $msgs_to_print[] = $msg[1];
         }
         if($msg[0] != USER_ERROR && $msg[0] != USER_WARNING && $msg[0] != USER_MSG)
         {
            $msgs_to_log[] = $msg[1];
         }
      }
      if(count($msgs_to_log) == 0)
      {
         $this->resetUserMessages();
         //return;
      }
      foreach($msgs_to_log as $log_msg)
      {
         logMessage($log_msg, 'sitelog.txt');
      }
      if(count($msgs_to_print) == 0)
      {
         $this->resetUserMessages();
         return;
      }
      echo "<ul class='admin-usermsg'>";
      foreach($msgs_to_print as $print_msg)
      {
          echo "<li class='error'>";
          echo $print_msg;
          echo "</li>";
      }
      echo "</ul>";
      $this->resetUserMessages();
   }

   function getNotifyForm($title, $warning, $action, $type, $id, $reqd, $buttonhtml)
   {
      $str = "<div class='editorconfirm'>\n";
      $str .= '<div align="center" class="confirmbox">';
      $str .= "<p class='confirmheader'>$title</p>\n";
      $str .= '<p align="center"><img src="graphics/caution.gif" align=center><br />';
      $str .= "$warning</p>\n";
      $str .= '</div>';
      $str .= "<div class='notifybox'>\n";
      $str .= '<img src="graphics/mailicon.gif" valign=middle> <b>Send Mail Notification </b>
            <input type="checkbox" name="send_mail" ';
      if($reqd) $str .= 'checked DISABLED>';
      else $str .= "checked>";
      $str .= '<br /><br />';
      $str .= '<B>Reason for taking action '.$action.'</B><br />';
      $str .= '<TEXTAREA name=action_reason rows=5 cols=30></TEXTAREA>';
      $str .= $buttonhtml;
      $str .= "</div></div>\n";
      return $str;
   }

}

?>
