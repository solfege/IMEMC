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

class Editorial
{
    var $no_rows;
    var $query_count;
    var $action_id;
    var $action_time;
    var $editor_name;
    var $action;
    var $content_type;
    var $editor_reason;
    var $content_id;
    var $secondary_id;
    var $item_title;

    function getNumberEditorialRows($filter_editor ="", $filter_type="", $filter_item="", $filter_id)
    {
       global $prefix, $dbconn;
      
       $grand_total = 0;

       $count_query = "SELECT count(*) from ".$prefix."_editorial_actions ";

       // If filters active, paging is relative to them, so total is different
       $count_query = $this->handleWhereClauses($count_query, $filter_editor, $filter_type, $filter_item, $filter_id);

       // echo("query ".$count_query. "<BR>");
       $result = sql_query($count_query , $dbconn, 1);
       checkForError($result);
       if(sql_num_rows( $result ) > 0)
       {
           list($grand_total) = sql_fetch_row($result, $dbconn);
           $this->no_rows = $grand_total;
           return $grand_total;
       }
       else
       {
           reportError("No rows present in editorial actions table");
       }
       return 0;
    }
    function resetArrays()
    {
       $this->query_count = 0;
       $this->action_id = array();
       $this->action_time = array();
       $this->editor_name = array();
       $this->action = array();
       $this->content_type = array();
       $this->editor_reason = array();
       $this->content_id = array();
       $this->secondary_id = array();
       $this->item_title = array();
    }

    function handleWhereClauses($editorial_query ="", $filter_editor ="", $filter_type="", $filter_item="",$filter_id="")
    {
       $where_conditions = array();
       if ($filter_editor != "")
       {
           array_push($where_conditions, "editor_name='".$filter_editor."' ");
       }
       if ($filter_type != "")
       {
           if ($filter_type == "hide_unhide") array_push($where_conditions," (action='hide' OR action='unhide') ");
	   else if ($filter_type == "unban") array_push($where_conditions, " (action='unban' OR action='name unban') ");
	   else if ($filter_type == "ban") array_push($where_conditions,   " (action='ban' OR action='name ban') ");
	   else array_push($where_conditions, "action='".$filter_type."' ");
       }
       if ($filter_item != "")
       {
           array_push($where_conditions, "content_type='".$filter_item."' ");
       }
       if ($filter_id != "")
       {
           array_push($where_conditions, "content_id='".$filter_id."' ");
       }

       if (count($where_conditions) > 0) $editorial_query = $editorial_query." WHERE ";
       for ($i=0; $i < count($where_conditions); $i++)
       {
          $editorial_query = $editorial_query.$where_conditions[$i];
          if ($i < count($where_conditions)-1) $editorial_query = $editorial_query." AND ";
       }

       return $editorial_query;
    }

    function selectEditorialRows($start_limit, $page_size, $filter_editor ="", $filter_type="", $filter_item="",$filter_id="")
    {
       global $prefix, $dbconn;
       $editorial_query = "SELECT action_id, UNIX_TIMESTAMP(action_time), editor_name, action, content_type, editor_reason, content_id, secondary_id from ".$prefix."_editorial_actions";

       $editorial_query = $this->handleWhereClauses($editorial_query, $filter_editor, $filter_type, $filter_item, $filter_id);

       if ($start_limit == 1) $start_limit = 0;
       $editorial_query .= " ORDER BY action_time DESC LIMIT ".$start_limit.", ".$page_size;

       // echo("Query: ".$editorial_query."<BR>");

       $result = sql_query($editorial_query, $dbconn, 1);
       checkForError($result);
       if(sql_num_rows( $result ) > 0)
       {
          $this->resetArrays();
          $this->query_count = sql_num_rows( $result ); 
          for ($i=0; $i<sql_num_rows( $result ); $i++)
          {
             list($this->action_id[$i], $this->action_time[$i],$this->editor_name[$i],$this->action[$i], $this->content_type[$i], $this->editor_reason[$i], $this->content_id[$i], $this->secondary_id[$i]) = sql_fetch_row($result, $dbconn);
             if($this->action[$i] == "ban" || $this->action[$i] == "unban") 
	         $this->content_id[$i] = convertNumberToIP($this->content_id[$i]);
          }
       }
       return true;
   }

    function generateQuery($start_limit, $page_size, $filter_type="hide", $filter_item="comment")
    {
       global $prefix, $dbconn;
       $editorial_query = "SELECT action_id, UNIX_TIMESTAMP(action_time), editor_name, action, content_type, editor_reason, content_id, secondary_id, ";

       if ($filter_item == "comment")
       {
           $editorial_query .= " c.comment_title from ".$prefix."_editorial_actions a, ".$prefix."_comments c where c.comment_id=a.secondary_id AND content_type='comment' AND ";
       }
       else if ($filter_item == "story" )
       {
           $editorial_query .= " s.story_title from ".$prefix."_editorial_actions a, ".$prefix."_stories s where s.story_id=a.content_id AND content_type='story' AND ";
       }

       if ($filter_type != "")
       {
           if ($filter_type == "hide_unhide") $editorial_query .= " action='hide' OR action='unhide' ";
	   else if ($filter_type == "hide") $editorial_query .= " action='hide' ";
       }


       if ($start_limit == 1) $start_limit = 0;
       $editorial_query .= " ORDER BY action_time DESC LIMIT ".$start_limit.", ".$page_size;

       return $editorial_query;
    }

    function selectHiddenItems($start_limit, $page_size, $filter_type="hide", $filter_item="comment")
    {
       global $prefix, $dbconn;

       if ($filter_item == "comment" || $filter_item == "story")
       {
           $editorial_query = $this->generateQuery($start_limit, $page_size, $filter_type, $filter_item );
       }
       else if ($filter_item == "both" )
       {
           $editorial_query = "(";
           $editorial_query .= $this->generateQuery($start_limit, $page_size, $filter_type, "comment" );
           $editorial_query .= ") UNION (";
           $editorial_query .= $this->generateQuery($start_limit, $page_size, $filter_type, "story" );
	   // Order by 2nd displayed item.
           $editorial_query .= ") ORDER BY 2 DESC";
       }

       // echo("<br><b>Filter Type:</b> ".$filter_item."<BR>");
       // echo("Query: ".$editorial_query."<BR>");

       $result = sql_query($editorial_query, $dbconn, 2);
       checkForError($result);
       if(sql_num_rows( $result ) > 0)
       {
          $this->resetArrays();
          $this->query_count = sql_num_rows( $result ); 
          for ($i=0; $i<sql_num_rows( $result ); $i++)
          {
             list($this->action_id[$i], $this->action_time[$i],$this->editor_name[$i],$this->action[$i], $this->content_type[$i], $this->editor_reason[$i], $this->content_id[$i], $this->secondary_id[$i],$this->item_title[$i]) = sql_fetch_row($result, $dbconn);
          }
       }
       return true;
   }
}


?>
