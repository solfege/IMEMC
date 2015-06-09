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

// This class is used for the form validation of the encoded data. Ideally it should be moved
// to its own file and it can be expaned upon from there.
class formValidator
{
   var $hidden_data;
   var $pos_data;
   var $value_mask;
   var $value_pos;
   var $number_fields;
   var $entry_window;
   var $current_sec;
   var $current_hour;
   var $timefrag_len;
   var $hour_array;
   var $failure_code;
   var $failure_info;
   var $is_email_form;

   function formValidator($use_set=1)
   {
      global $system_config;
      // This class generates 3 hidden values for the form. At least 2 have to be valid
      // for the form to be processed. Either 1 and 2 or 2 and 3 or 1, 2 and 3.
      // To change every day, day number can be got with strftime("%j", time());
      $t_day_index =  (int) strftime("%d", time()+$system_config->timezone_offset);
      $t2_day_index = $t_day_index + 8;
      if ($t2_day_index > 31 ) $t2_day_index = ($t2_day_index - 31);
      if ($t_day_index == null) $t_day_index = 2;

      $formfield = array( 1 => "fieldaddr", 2 => "fieldname", 3 => "feldarea", 4 => "felddata", 5 => "sessfeld",
                          6 => "nameaddr",  7 => "addr_name", 8 => "fielddat", 9 => "datayear", 10 => "seesfeld",
                          11=> "main_addr",12 => "gatedname",13 => "corearea", 14 => "day_data", 15 => "FeldSess",
                          16=> "addrxaddr",17 => "datadatae",18 => "thisdata", 19 => "datadate", 20 => "namefeld",
                          21=> "datumaddr",22 => "addrdayno",23 => "monthlyd", 24 => "Tagedata", 25 => "sessaddr",
			  26=> "fieldings",27 => "accessdat",28 => "feldxarea",29 => "opt_data", 30 => "option01",
			  31=> "garldings");

      $mandatory_tag = "mand" .$t2_day_index. $formfield[$t2_day_index];
      if ($use_set == 1) {
          if ( ($t_day_index % 2) == 0) {
              $this->hidden_data = array( 1 => $formfield[$t_day_index], 2 => "yourname", 3 => "optionaldata", 4 => $mandatory_tag, 5 => "sessionid" );
	  } else {
              $this->hidden_data = array( 1 => $mandatory_tag, 2 => "yourname", 3 => "optionaldata", 4 => $formfield[$t_day_index], 5 => "sessionid" );
	  }
          $this->is_email_form = false;
      } else {
          $this->hidden_data = array( 1 => "sessaddr", 2 => "namefield", 3 => "mandatorydata", 4 => "optionaldata", 5 => "datafield" );
          $this->is_email_form = true;
      }
      $this->prefix_len = 6;
      $this->pos_data = $this->prefix_len;
      $this->timefrag_len = 6;
      $t_total = $this->timefrag_len + 5 + $this->prefix_len;
      //                                   23       23       23       12      23 
      $this->hidden_data_len = array( 1 => 23, 2 => 23, 3 => 23, 4 => $t_total, 5=> 23 );
      // Use 24 unique characters here. Must be unique. Use lettes or numbers.
      //                   123456789012345678901234
      $this->value_mask = "x1A83hjlprtYIc5ws0aZLGu2";
      //                   1234567890123456789012345678901234567890
      $this->dummy_mask = "ahj7834HORYV8nvzapre3RxR324gvCbpl1301twqK";
      // This must be alpha chars only for the moment.
      $this->alpha_data = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
      $this->alpha_mask = "defghijklNnopqrvwzyxABCREFGsutHILKJMnOPQDSTUVWXYZabc";
      //$this->alpha_mask="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
      $this->value_pos = 5 - 1;
      $this->number_fields = 5;
      // If it is filled out quicker than 8 secs, it's probably auto spam.
      $this->entry_window = 2;
      if ($use_set == 1) $this->entry_window = 8;
      // Make it 1 based
      $this->current_sec = substr(time(), (strlen(time()) -$this->timefrag_len),$this->timefrag_len);
      $this->current_hour = strftime("%H", time()) + 1;
      $this->hour_array = array_fill(1, 3, $this->current_hour);
      $this->failure_code = 0;
      
      // $thedata ="a1 " . $this->hour_array[1] . " a2 ".$this->hour_array[2] . " a3 ".$this->hour_array[3] ; 

      if ($this->current_hour > 1 ) $this->hour_array[1]--; else $this->hour_array[1] = 24;

      if ($this->current_hour > 23 ) $this->hour_array[3]=1; else $this->hour_array[3]++;
   }

   function getRndSequenceString($number_of_digits)
   {
      $encrypt_str = "";
      if ($number_of_digits <= 6) {
          switch ($number_of_digits) {
		case 0:
		case 1:
	                $encrypt_str = rand(0,9);
			break;
		case 2:
	                $encrypt_str = rand(10,99);
			break;
		case 3:
	                $encrypt_str = rand(100,999);
			break;
		case 4:
	                $encrypt_str = rand(1000,9999);
			break;
		case 5:
	                $encrypt_str = rand(10000,99999);
			break;
		case 6:
		default:
	                $encrypt_str = rand(100000,999999);
			break;
          }
      } else {
          for ($i = 1; $i <= $number_of_digits; $i++) {
	      $encrypt_str .= rand(0,9);
          }
      }
      return $encrypt_str;
   }

   function getRndEncryptedString($number_of_chars)
   {
      // This function generates 3 hidden values for the form. At least 2 have to be valid
      // for the form to be processed. Either 1 and 2 or 2 and 3 or 1, 2 and 3.
      
      $encrypt_str = "";
      for ($i = 1; $i <= $number_of_chars; $i++) {
	  $encrypt_str .= substr($this->dummy_mask,rand(0,40),1);
      }
      return $encrypt_str;
   }

   function getHiddenData()
   {
      // This function generates 3 hidden values for the form. At least 2 have to be valid
      // for the form to be processed. Either 1 and 2 or 2 and 3 or 1, 2 and 3.
      
      $t_current_sec = substr($this->current_sec,4,1) . substr($this->current_sec,0,4) . substr($this->current_sec,5,1); 

      $hidden_text = $this->getRndSequenceString($this->prefix_len) ."-". $t_current_sec ."-". rand(10000,99999) ;

      $hidden_text = "<input type=\"hidden\" name=\"" . $this->hidden_data[4] . "\" value=\"" . $this->getRndSequenceString($this->prefix_len) . $t_current_sec . rand(10000,99999) . "\">\n";

      for ($i = 1; $i <= 3; $i++) {
	  $val_str = $this->encodeNumber(rand(1000,9999)) . $this->encodeNumber($this->hour_array[$i], 1);
	  $val_str .= $this->getRndEncryptedString(18);
          $hidden_text .= "<input type=\"hidden\" name=\"" . $this->hidden_data[$i] . "\" value=\"" . $val_str . "\">\n";
      }

      $val_str = $this->encodeNumber(rand(1000,9999)) ;
      $val_str .= $this->encodeMethod();
      $val_str .= $this->getRndEncryptedString(15);
      $hidden_text .= "<input type=\"hidden\" name=\"" . $this->hidden_data[5] . "\" value=\"" . $val_str . "\">\n";


      return $hidden_text;
   }

   function rewriteHiddenData()
   {
      // This takes the existing data and just fills out the input fields again.
      // for the form to be processed. Either 1 and 2 or 2 and 3 or 1, 2 and 3.
      
      $hidden_text = "";

      for ($i = 1; $i <= 5; $i++) {
	  //echo("testing ".$this->hidden_data[$i] . "<br>");
	  $t_field = $this->hidden_data[$i];
          if (isset($_REQUEST[$t_field])) {
              $hidden_text .= "<input type=\"hidden\" name=\"" . $this->hidden_data[$i] . "\" value=\"" . $_REQUEST[$t_field] . "\">\n";
          }
      }

      return $hidden_text;
   }

   function encodeMethod()
   {
       // We happen to know it is 1 based, but string is zero based. Should be get so add x to make 4
       if (!isset($_SERVER['REQUEST_METHOD'])) $enc_str = "NONE";
       else $enc_str = $_SERVER['REQUEST_METHOD'] ;
       if ($enc_str == "GET") $enc_str .= "X";

       // Is it alphachar
       $encrypt_str = "";
       if (ctype_alpha($enc_str)) {
           if (strlen($enc_str) > 4 ) $enc_str = "LONG";
           if (strlen($enc_str) <= 3 ) $enc_str = "SHRT";

           $encrypt_str = "";
           for ($i = 0; $i <= 3; $i++) {
               $str_pos = strpos($this->alpha_data,substr($enc_str,$i,1));
               // Note the triple equals -see manual
               if ($str_pos === FALSE) {
	           $encrypt_str = "AAAA";
	           break;
               }
               $encrypt_str .= substr($this->alpha_mask, $str_pos,1);
           }
       } else {
           return $enc_str;
       }

       return $encrypt_str;
   }

   function decodeMethod($encdata_str)
   {

       $decrypt_str = "";
       // Note: The less than whereas above it is less than or equal because we are sure of the length
       for ($i = 0; $i < strlen($encdata_str); $i++) {
           $str_pos = strpos($this->alpha_mask,substr($encdata_str,$i,1));
           // Note the triple equals -see manual
           if ($str_pos === FALSE) {
               $decrypt_str = "CCCC";
	       break;
           }
           $decrypt_str .= substr($this->alpha_data, $str_pos,1);
       }

       return $decrypt_str;
   }

   function encodeNumber($target_num, $hour_mode =0)
   {
       if ($hour_mode == true )
       {
	   // We happen to know it is 1 based, but string is zero based.
           $ret_str = substr($this->value_mask,($target_num-1),1);
       } 
       else 
       {
           $ret_str = "";
           // Each char must be a digit.
           for ($i = 0; $i < strlen($target_num); $i++) {
               $ret_str .= substr($this->value_mask,substr($target_num,$i,1),1);
           }
       }

       return $ret_str;
   }
   function decodeNumberMask($target_char)
   {
       // Incoming char indexes into mask. And return pos will be zero based.

       $pos = strpos($this->value_mask, $target_char);
       // Read note about usage of strpos in PHP manual
       if ($pos === false) return -1;
       else return ($pos+1);
   }

   function reportInvalidation( $object_id)
   {
      global $system_config;
      // Log it to the spam log. 
      // $msg = "";
      $msg = "Reasons: Likely to be spam as: " ;
      if ($this->failure_code == 1 ) 
      {
	  $msg = "Form posted inside time window. Form data:";
          if ($this->failure_info != null ) $msg .= " " . $this->failure_info . " ";
      }
      else
      {
	  $msg = "Form data invalid Reason(" . $this->failure_code . "):";
          if ($this->failure_code >= 8 ) $msg .= " Prev REQ-METHOD= (" . $this->failure_info . ") ";
      }

      for ($i = 1; $i <= $this->number_fields; $i++) {
          if (isset($_REQUEST[$this->hidden_data[$i]])) 
          {
              $msg .= " Field " . $this->hidden_data[$i] . " Value: " . $_REQUEST[$this->hidden_data[$i]];
          }
	  else $msg .= " Field " . $this->hidden_data[$i] . " NOT set. ";
      }

      if(!isset($_SERVER['HTTP_REFERER']))
      {
	  $msg .= "HTTP_REFERER not set. ";
      }
      else
      {
	  $msg .= "HTTP_REFERER=" . $_SERVER['HTTP_REFERER'] . " ";
      }

      if(!isset($_SERVER['HTTP_USER_AGENT']))
      {
          $msg .= "HTTP_USER_AGENT NOT set. ";
      } else {
	  $msg .= "HTTP_USER_AGENT=" . $_SERVER['HTTP_USER_AGENT'] . " ";
      }


      if (isset($_SERVER['REQUEST_METHOD']))
      {
          $msg .= " REQUEST_METHOD=" . $_SERVER['REQUEST_METHOD'];
      }
      if (isset($_SERVER['REQUEST_URI']))
      {
          $msg .= " REQUEST_URI=" . $_SERVER['REQUEST_URI'];
      }
      if (isset($_SERVER['REDIRECT_URL']))
      {
          $msg .= " REDIRECT_QUERY=" . $_SERVER['REDIRECT_URL'];
      }
      $msg .= " Note: obj_id: " . $object_id;


      if ($this->is_email_form == true) {
          $subject = (isset($_REQUEST['subject'])) ? $_REQUEST['subject'] : "";
          $name = (isset($_REQUEST['name'])) ? htmlspecialchars($_REQUEST['name']) : "";
          $email = (isset($_REQUEST['email'])) ? htmlspecialchars($_REQUEST['email']) : "";
          $message = (isset($_REQUEST['message'])) ? htmlspecialchars($_REQUEST['message']) : "";
    
          // We may want to change this to show the actual form fields filled out.
          $msg .= "\nForm fields are:\nSubject: " . $subject . "\nEmail: " . "\"" . $name . "\" " . $email ;
          $msg .= "\nMsgText: " . $message;
      }

      $ref_number = time();
      $head_msg = "--Start_Post-" . $ref_number ."--Time_Sent:". date("D, d M Y H:i:s",(time()+$system_config->timezone_offset))."\n"; 
      $final_msg = $head_msg . $msg . "\n--End_Post-" . $ref_number ."--\n";

      logSpammerMessage($final_msg, "spamlog_contact.txt", false);

   }
   function validateFormData()
   {
      // This will automatically search for the data in $_REQUEST starting with time info

      $this->failure_code = 3;
      if (!isset($_REQUEST[$this->hidden_data[4]]) ) return false;

      $this->failure_code = 0;

      $t_sec = substr($_REQUEST[$this->hidden_data[4]],$this->pos_data, $this->timefrag_len);
      $t_decoded_sec = substr($t_sec,1,4) . substr($t_sec,0,1) . substr($t_sec,5,1); 

      // if (substr($_REQUEST[$this->hidden_data[4]],$this->pos_data, $this->timefrag_len) > ($this->current_sec - $this->entry_window) )
      if ($t_decoded_sec > ($this->current_sec - $this->entry_window) )
      {
          $this->failure_info = "Data " .substr($_REQUEST[$this->hidden_data[4]],$this->pos_data, $this->timefrag_len) . " greater than ".($this->current_sec - $this->entry_window) . " current_sec " . $this->current_sec . " entry " . $this->entry_window;
          $this->failure_code = 1;
          return false;
      }

      // Make sure the fields are the correct lengths

      for ($i = 1; $i <= $this->number_fields; $i++) {
          if (isset($_REQUEST[$this->hidden_data[$i]])) 
          {
              if (strlen($_REQUEST[$this->hidden_data[$i]]) != $this->hidden_data_len[$i])
              {
                  if ($i== 5) $this->failure_code = 6;
		  else $this->failure_code = 4;
                  return false;
              }
          }
	  else
          {
              if ($i== 5) $this->failure_code = 7;
              else $this->failure_code = 3;
              return false;
          }
      }

      // Check the 5th field
      $field_data = substr($_REQUEST[$this->hidden_data[5]],$this->value_pos,4);

      if ( !ctype_alpha($field_data) ) {
          $this->failure_code = 8;
          $this->failure_info = $field_data;
          return false;
      }

      if ( $field_data == "AAAA") {
          $this->failure_code = 9;
          $this->failure_info = $field_data;
          return false;
      }
      $decoded_field = $this->decodeMethod($field_data);
      if ( substr($decoded_field,0,3) != "GET") {
          $this->failure_code = 10;
          $this->failure_info = $decoded_field;
          return false;
      }


      // The 2nd one has to exist and if it does be the right value.
      // The match can be either all 3 or 2 and 3 against 1 and 2 here.

      if (!isset($_REQUEST[$this->hidden_data[2]]) ) return false;

      $decodedFormValue = $this->decodeNumberMask(substr($_REQUEST[$this->hidden_data[2]],$this->value_pos,1));

      if ($decodedFormValue != $this->hour_array[2] ) 
      {
          // Then check if it matches to 1 with 3
          if ($decodedFormValue == $this->hour_array[1] ) 
          {
              if (!isset($_REQUEST[$this->hidden_data[3]])) return false;

              $decodedFormValue = $this->decodeNumberMask(substr($_REQUEST[$this->hidden_data[3]],$this->value_pos,1));
              if ($decodedFormValue == $this->hour_array[2] ) return true;
          }

          return false;
      }

      // Both have to match.
      if (isset($_REQUEST[$this->hidden_data[1]]) )
      { 
          $decodedFormValue = $this->decodeNumberMask(substr($_REQUEST[$this->hidden_data[1]],$this->value_pos,1));
          if ($decodedFormValue == $this->hour_array[1] )
          {
              if (isset($_REQUEST[$this->hidden_data[3]]) )
              { 
                  $decodedFormValue = $this->decodeNumberMask(substr($_REQUEST[$this->hidden_data[3]],$this->value_pos,1));
                  if ($decodedFormValue == $this->hour_array[3] ) return true;
              }
          }
      } 

      $this->failure_code = 2;
      return false;
   }
}
?>
