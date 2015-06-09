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

   class Ticket
   {
      var $use_mask;
      var $used_mask;
      var $wait_mask;
      var $mask_length;
      var $unique_id = NULL;
      var $time_posted;
      var $is_posted = 0;
      # Time between successive publishes in seconds -default is 5 mins
      var $blocking_time;
      var $writing_time;
      var $ticket_type;
      var $min_t;

      function Ticket( $publish_news="c")
      {
         global $editor_session;

         # This data could easily be made configurable to make more secure.
         $this->use_mask = 'AM';
         $this->used_mask = 'BF';
         $this->wait_mask = 'WJ';
         $this->mask_length = 2;
         $this->min_t = 3;

         # In editorsession object, this is assigned if present as a cookie.
         # Block time is that limits gaps between publishing comments while
         # writing time is the window time by which you better write up your comment
              $this->blocking_time = 100;
              $this->writing_time = 3600;
              $this->is_posted = 0;
         $this->unique_id = NULL;

         // Comment = 0, News = 1
         $this->ticket_type = 0;

         if ( strcmp($publish_news,"n") == 0 )
         {
            $this->use_mask = 'TR';
            $this->used_mask = 'GX';
            $this->wait_mask = 'HN';
            $this->blocking_time = 3600;
            $this->writing_time = 3600;
            $this->ticket_type = 1;
            if ($editor_session->newstoken != null)
            {
                  $this->unique_id = $editor_session->newstoken;
            }
         }
         elseif ($editor_session->token != null)
         {
            $this->unique_id = $editor_session->token;
         }
      }

      function setTimeouts($blocking_time, $writing_time)
      {
         $this->blocking_time = $blocking_time;
         $this->writing_time = $writing_time;
      }

      function invalidateTicket()
      {
         global $editor_session;
         # If this is called, assumption is that the unique id is the right one and
         # that it's not null. Should never be not null. Should add a log msg in a else statement
         $this->is_posted = true;
         if($this->unique_id != NULL)
         {
            if ($this->ticket_type == 1)
            {
               $editor_session->newstoken = $this->used_mask . (string) substr($this->unique_id, $this->mask_length);
               $_SESSION['newstoken'] = $editor_session->newstoken;
               $this->unique_id = $editor_session->newstoken;
            }
            else
            {
               $editor_session->token = $this->used_mask . (string) substr($this->unique_id, $this->mask_length);
               $_SESSION['token'] = $editor_session->token;
               $this->unique_id = $editor_session->token;
            }
         }
      }

      function wasTicketUsed($form_ticket_id)
      {
         global $editor_session;

         $ticket_used = false;
         if( $editor_session->isSessionOpen())
         {
            return $ticket_used;
         }

         if($form_ticket_id != NULL)
         {
            if ( strcmp( substr($form_ticket_id, 0, $this->mask_length), $this->used_mask) == 0)
            {
               $ticket_used = true;
            }
         }

         return $ticket_used;
      }

      function isTicketWaiting($form_ticket_id)
      {
         global $editor_session;

         $ticket_waiting = false;
         if( $editor_session->isSessionOpen())
         {
            return $ticket_waiting;
         }

         if($form_ticket_id != NULL)
         {
            if ( strcmp( substr($form_ticket_id, 0, $this->mask_length), $this->wait_mask) == 0)
            {
               $ticket_waiting = true;
            }

         }
         return $ticket_waiting;
      }

      function isTicketStillValid($form_ticket_id)
      {
         global $editor_session, $system_config;

         $ticket_valid = false;
         if( $editor_session->isSessionOpen())
         {
            return true;
         }

         if($form_ticket_id != NULL)
         {
            if ( strcmp( substr($form_ticket_id, 0, $this->mask_length), $this->use_mask) == 0)
            {
               $numeric_part = substr($form_ticket_id, $this->mask_length);
               $corrected_time = time()+$system_config->timezone_offset -$this->writing_time;
               if ( $numeric_part > $corrected_time)
               {
                  $ticket_valid = true;
               }
            }
         }

         return $ticket_valid;
      }

      function getTicket()
      {
         return $this->unique_id;
      }

      function isTicketTimeGapValid()
      {
         global $system_config, $editor_session;

         if( $editor_session->isSessionOpen())
         {
            return true;
         }
         if ($this->unique_id == null )
         {
            return false;
         }
         // Check to see if ticket is still within the publishing time interval.
         if ( strcmp( substr($this->unique_id, 0, $this->mask_length), $this->wait_mask) == 0)
         {
            return false;
         }

         if($this->ticket_type == 1)
         {
            $this->time_posted = substr($editor_session->newstoken, strlen($this->use_mask));
	    // echo("Time posted ". $this->time_posted."<BR>");
	    // echo("Time posted ". (time() + $system_config->timezone_offset - $this->time_posted) . "<BR>");
            if ((time() + $system_config->timezone_offset - $this->time_posted) < $this->min_t) 
	    {
                return false;
	    }
         }

         return true;
      }

      function blockedTimeRemaining()
      {
         global $system_config, $editor_session;
         if($this->ticket_type == 1)
         {
            $this->time_posted = substr($editor_session->newstoken, strlen($this->use_mask));
         }
         else
         {
            $this->time_posted = substr($editor_session->token, strlen($this->use_mask));
         }
         $block = $this->blocking_time  - (time() + $system_config->timezone_offset - $this->time_posted);
         if($block <= 0) return 0;
         return $block;
      }

      function generateNewTicket()
      {
         // Only generate a new token if the last one is more than 5 mins (blocking_time) old
         global $system_config, $editor_session;
         $ticket_generated = false;
         if ($this->unique_id == null )
         {
            $this->time_posted = time()+$system_config->timezone_offset;
            $this->is_posted = 0;
            if ($this->ticket_type == 1)
            {
               $editor_session->newstoken = $this->use_mask . (string) $this->time_posted;
               $this->unique_id = $editor_session->newstoken;
               $_SESSION['newstoken'] = $editor_session->newstoken;
            }
            else
            {
               $editor_session->token = $this->use_mask . (string) $this->time_posted;
               $this->unique_id = $editor_session->token;
               $_SESSION['token'] = $editor_session->token;
            }
            $ticket_generated = true ;
         }
         elseif ((!$this->isTicketWaiting($this->unique_id) && !$this->wasTicketUsed($this->unique_id)) || (substr($this->unique_id,$this->mask_length) + $this->blocking_time) < time()+$system_config->timezone_offset)
         {
            $this->time_posted = time()+$system_config->timezone_offset;
            $this->is_posted = 0;

            if ($this->ticket_type == 1)
            {
               $editor_session->newstoken = $this->use_mask . (string) $this->time_posted;
               $_SESSION['newstoken'] = $editor_session->newstoken;
               $this->unique_id = $editor_session->newstoken;
            }
            else
            {
               $editor_session->token = $this->use_mask . (string) $this->time_posted;
               $_SESSION['token'] = $editor_session->token;
               $this->unique_id = $editor_session->token;
            }
            $ticket_generated = true ;
         }
         else
         {

            if ($this->ticket_type == 1)
            {
               $ticket_valid = $this->isTicketStillValid($editor_session->newstoken);
            }
            else
            {
               $ticket_valid = $this->isTicketStillValid($editor_session->token);
            }

            if(isset($_REQUEST['number_of_files']) && ($_REQUEST['number_of_files'] > 0) && ($ticket_valid == true))
            {
               # Not really
               $ticket_generated = true ;
            }
            else
            {
               if ($this->ticket_type == 1)
               {
                  $editor_session->newstoken = $this->wait_mask . (string) substr($this->unique_id, $this->mask_length);
                  $_SESSION['newstoken'] = $editor_session->newstoken;
                  $this->unique_id = $editor_session->newstoken;
               }
               else
               {
                  $editor_session->token = $this->wait_mask . (string) substr($this->unique_id, $this->mask_length);
                  $_SESSION['token'] = $editor_session->token;
                  $this->unique_id = $editor_session->token;
               }
            }
         }

         if( $editor_session->isSessionOpen())
         {
            $ticket_generated = true ;
         }

        # else the ticket is just not generated -have to time out
         return $ticket_generated;
      }
   }
?>
