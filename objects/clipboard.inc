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

class Clipboard
{
   var $features = array();
   var $stories = array();
   var $comments = array();
   var $image_attachments = array();
   var $video_attachments = array();
   var $audio_attachments = array();
   var $miscellaneous_attachments = array();

   function clip($item)
   {
      if(isset($item->attachment_id) && $item->attachment_id!=null)
      {
         if($item->isImage())
         {
            if($this->isAlreadyClipped($item->attachment_id,$this->image_attachments)==false)
            {
               array_push($this->image_attachments,$item->attachment_id);
            }
         }
         else if($item->isVideo())
         {
            if($this->isAlreadyClipped($item->attachment_id,$this->video_attachments)==false)
            {
               array_push($this->video_attachments,$item->attachment_id);
            }
         }
         else if($item->isAudio())
         {
            if($this->isAlreadyClipped($item->attachment_id,$this->audio_attachments)==false)
            {
               array_push($this->audio_attachments,$item->attachment_id);
            }
         }
         else
         {
            if($this->isAlreadyClipped($item->attachment_id,$this->miscellaneous_attachments)==false)
            {
               array_push($this->miscellaneous_attachments,$item->attachment_id);
            }
         }
      }
      else if(isset($item->comment_id) && $item->comment_id !=null)
      {
         if($this->isAlreadyClipped($item->comment_id,$this->comments)==false)
         {
            array_push($this->comments,$item->comment_id);
         }
      }
      else if(isset($item->story_id) && $item->story_id!=null && $item->type_id!=1)
      {
         if($this->isAlreadyClipped($item->story_id,$this->stories)==false)
         {
            array_push($this->stories,$item->story_id);
         }
      }
      else if(isset($item->story_id) && $item->story_id!=null && $item->type_id==1)
      {
         if($this->isAlreadyClipped($item->story_id,$this->features)==false)
         {
            array_push($this->features,$item->story_id);
         }
      }
   }
   function unclip($item)
   {
      if(isset($item->attachment_id) && $item->attachment_id!=null)
      {
         if($item->isImage()) $this->removeClip($this->image_attachments,$item->attachment_id);
         else if($item->isVideo()) $this->removeClip($this->video_attachments,$item->attachment_id);
         else if($item->isAudio()) $this->removeClip($this->audio_attachments,$item->attachment_id);
         else $this->removeClip($this->miscellaneous_attachments,$item->attachment_id);
      }
      else if(isset($item->comment_id) && $item->comment_id!=null) $this->removeClip($this->comments,$item->comment_id);
      else if(isset($item->story_id) && $item->story_id!=null && $item->type_id!=1) $this->removeClip($this->stories,$item->story_id);
      else if(isset($item->story_id) && $item->story_id!=null && $item->type_id==1) $this->removeClip($this->features,$item->story_id);
   }

   function isAlreadyClipped($item_id,$item_array)
   {
      for($i=0;$i<count($item_array);$i++)
      {
         if($item_array[$i]==$item_id) return true;
      }
      return false;
   }

   function removeClip(&$item_array,$item_id)
   {
      for($i=0;$i<count($item_array);$i++)
      {
         if($item_array[$i]==$item_id) array_splice($item_array,$i,1);
      }
   }

   function writeClipboard(&$query)
   {
      global $graphics_store, $oscailt_page_setting, $editor_session;
      if((!isset($editor_session->editor->editor_state['edit_mode'])) or ($editor_session->editor->editor_state['edit_mode'] != 'content' and $editor_session->editor->editor_state['edit_mode'] != 'all')) return;
      if(!isset($oscailt_page_setting['obj_type']) or !isset($oscailt_page_setting['obj_id']) or !isset($oscailt_page_setting['site_module_settings']))
      {
         $page_setting_error = true;
      }
      else
      {
         $page_setting_error = false;
         $page_type = strtolower($oscailt_page_setting['obj_type']);
      }
      if(isset($_REQUEST['print_page'])) return;
      ?>
      <p>
      <table class=admin align=center>
      <tr class=admin>
         <th class=admin colspan=3><img src="<?=$graphics_store?>clip.gif">Clipboard</th>
      </tr>
      <tr class=admin>
         <td class=admin colspan=3 align=center><i>You can use this clipboard to save links to items as you traverse the newswire. Depending upon what page you're on the links will have different actions associated with them.</i></td>
      </tr>
      <?
      if($page_setting_error)
      {
         ?>
         <tr class=admin>
         <td class=admin colspan=3 align=center><span class=error>You cannot use the clipboard as the page settings have not been properly configured</span></td>
         </tr>
         <?
      }
      elseif($page_type != 'article' && $page_type != 'publish' && $page_type != 'newswire' && $page_type != 'feature')
      {
         ?>
            <tr class=admin>
            <td class=admin colspan=3 align=center><span class=error>A clipboard can't do anyting on this type of page - why is it here?</span></td>
            </tr>
         <?
      }
      else
      {
         $this->writeFeatureClipboard($query);
         $this->writeStoryClipboard($query);
         $this->writeCommentClipboard($query);
         $this->writeAttachmentClipboard($query,"Image Files","imageicon.gif",$this->image_attachments);
         $this->writeAttachmentClipboard($query,"Video Files","videoicon.gif",$this->video_attachments);
         $this->writeAttachmentClipboard($query,"Audio Files","audioicon.gif",$this->audio_attachments);
         $this->writeAttachmentClipboard($query,"Miscellaneous Files","miscicon.gif",$this->miscellaneous_attachments);
      }
      ?>
      </table>
      <?
   }
   function writeFeatureClipboard(&$query)
   {
      global $oscailt_page_setting, $OSCAILT_SCRIPT, $pseudo_directory_offset;
      $article_url = $pseudo_directory_offset.$OSCAILT_SCRIPT.'?obj_id='.$oscailt_page_setting['site_module_settings']['articleobject'];
      if(count($this->features)>0) $this->writeClipboardHeader("Features","texticon.gif");
      for($i=0;$i<count($this->features);$i++)
      {
         $story = new Story();
         $story->story_id=$this->features[$i];
         $story->load();
         $this->writeClipboardItem($query,$story,$story->story_id,"feature",$story->story_title." by ".$story->author_name ,$article_url.'&amp;story_id='.$story->story_id);
      }
   }
   function writeStoryClipboard(&$query)
   {
      global $oscailt_page_setting, $pseudo_directory_offset, $OSCAILT_SCRIPT;
      $article_url = $pseudo_directory_offset.$OSCAILT_SCRIPT.'?obj_id='.$oscailt_page_setting['site_module_settings']['articleobject'];
      if(count($this->stories)>0) $this->writeClipboardHeader("Stories","texticon.gif");
      for($i=0;$i<count($this->stories);$i++)
      {
         $story = new Story();
         $story->story_id=$this->stories[$i];
         $story->load();
         $this->writeClipboardItem($query,$story,$story->story_id,"story",$story->story_title." by ".$story->author_name ,$article_url.'&amp;story_id='.$story->story_id);
      }
   }

   function writeCommentClipboard(&$query)
   {
      global $oscailt_page_setting, $OSCAILT_SCRIPT, $pseudo_directory_offset;
      $article_url = $pseudo_directory_offset.$OSCAILT_SCRIPT.'?obj_id='.$oscailt_page_setting['site_module_settings']['articleobject'];
      if(count($this->comments)>0) $this->writeClipboardHeader("Comments","texticon.gif");
      for($i=0;$i<count($this->comments);$i++)
      {
         $comment = new Comment();
         $comment->comment_id=$this->comments[$i];
         $comment->load();
         $this->writeClipboardItem($query,$comment,$comment->comment_id,"comment",$comment->comment_title." by ".$comment->author_name ,$article_url.'&amp;story_id='.$comment->story_id."#comment".$comment->comment_id);
      }
   }

   function writeAttachmentClipboard(&$query,$name,$icon,&$attachments)
   {
      if(count($attachments)>0) $this->writeClipboardHeader($name,$icon);
      for($i=0;$i<count($attachments);$i++)
      {
         $attachment = new Attachment();
         $attachment->attachment_id=$attachments[$i];
         $attachment->load();
         $this->writeClipboardItem($query,$attachment,$attachment->attachment_id,"attachment",$attachment->getDisplayName(),$attachment->getURL());
      }
   }

   function writeClipboardItem(&$query,$item,$item_id,$item_type,$name,$link)
   {
      ?>
         <tr class="admin">
            <td>&nbsp;<?=$item_id?>&nbsp;</td>
            <td>&nbsp;<a href="<?=$link?>"><?=$name?></a>&nbsp;</td>
            <td align=right>
            <?
            $this->writeClipboardOptions($query,$item,$item_id,$item_type);
            ?>
            </td>
         </tr>
      <?
   }

   function writeClipboardHeader($name,$icon)
   {
      global $graphics_store;
      ?>
         <tr class=admin>
            <th class=admin colspan=3><img src="<?=$graphics_store.$icon?>"><?=$name?></th>
         </tr>
      <?
   }


   function writeClipboardOptions(&$query,$item,$item_id,$item_type)
   {
      global $graphics_store, $editor_session,$system_config,$pseudo_directory_offset,$OSCAILT_SCRIPT,$oscailt_page_setting;
      $site_id = $oscailt_page_setting['site_id'];
      $page_type = strtolower($oscailt_page_setting['obj_type']);
      $article_url = $pseudo_directory_offset.$OSCAILT_SCRIPT.'?obj_id='.$oscailt_page_setting['site_module_settings']['articleobject'];

      if($editor_session->isSessionOpen())
      {

         if($item_type=="story" && $item->type_id==1) $item_type="feature";
         if($page_type == 'article' or $page_type == 'newswire' or $page_type == 'feature')
         {
            $link = $query->getQueryString();
            if($link == "") $link = $pseudo_directory_offset.$OSCAILT_SCRIPT.'?';
            else $link = $pseudo_directory_offset.$OSCAILT_SCRIPT.$link.'&amp;';
            if($query->comment_id!=null)
            {
               if($item_type=="attachment" && $query->comment_id!=$item->comment_id)
               {
                  if($editor_session->editor->allowedReadAccessTo("copy".$item_type, $site_id))
                  {
                     ?> <a href="<?=$link?>copy<?=$item_type?>=<?=$item_id?>"><img title="Copy <?=$item_type?> <?=$item_id?> to current comment" alt="Copy <?=$item_type?> <?=$item_id?> to current comment" src="<?=$graphics_store?>copy.gif" border=0></a><?
                  }
                  if($editor_session->editor->allowedReadAccessTo("move".$item_type, $site_id))
                  {
                     ?> <a href="<?=$link?>move<?=$item_type?>=<?=$item_id?>"><img title="Move <?=$item_type?> <?=$item_id?> to current comment" alt="Move <?=$item_type?> <?=$item_id?> to current comment" src="<?=$graphics_store?>move.gif" border=0></a><?
                  }
               }
            }

            else if($query->story_id!=null)
            {
               if(($item_type=="comment" || $item_type=="attachment") && $query->story_id!=$item->story_id)
               {
                  if($editor_session->editor->allowedReadAccessTo("copy".$item_type, $site_id))
                  {
                     ?> <a href="<?=$link?>copy<?=$item_type?>=<?=$item_id?>"><img title="Copy <?=$item_type?> <?=$item_id?> to current story" alt="Copy <?=$item_type?> <?=$item_id?> to current story" src="<?=$graphics_store?>copy.gif" border=0></a><?
                  }
                  if($editor_session->editor->allowedReadAccessTo("move".$item_type, $site_id))
                  {
                     ?> <a href="<?=$link?>move<?=$item_type?>=<?=$item_id?>"><img title="Move <?=$item_type?> <?=$item_id?> to current story" alt="Move <?=$item_type?> <?=$item_id?> to current story" src="<?=$graphics_store?>move.gif" border=0></a><?
                  }
               }

               else if($item_type=="story" && $query->story_id!=$item_id) // ensure story in clipboard and current story are not the same
               {
                  if($editor_session->editor->allowedReadAccessTo("downgrade".$item_type, $site_id))
                  {
                     ?> <a href="<?=$link?>downgrade<?=$item_type?>=<?=$item_id?>"><img title="Downgrade <?=$item_type?> <?=$item_id?> to a comment in current story" alt="Downgrade <?=$item_type?> <?=$item_id?> to a comment in current story" src="<?=$graphics_store?>downgrade.gif" border=0></a><?
                  }
               }
            }
            ?> <a href="<?=$link?>unclip<?=$item_type?>=<?=$item_id?>"><img title="Remove <?=$item_type?> <?=$item_id?> from clipboard" alt="Remove <?=$item_type?> <?=$item_id?> from clipboard" src="<?=$graphics_store?>delete.gif" border=0></a><?
         }
         else if($page_type == 'publish')
         {

            if($item_type=="story" || $item_type=="feature")
            {
               $linkurl=$article_url.'&amp;story_id='.$item->story_id;
               $linkname=$item->story_title;
            }
            else if($item_type=="comment")
            {
               $linkurl=$article_url.'&amp;story_id='.$item->story_id."&amp;comment_limit=0&amp;condense_comments=false#comment".$item->comment_id;
               $linkname=$item->comment_title;
            }
            else if($item_type=="attachment")
            {
               $linkurl=$item->getURL();
               $linkname=$item->getDisplayName();
            }
            ?>
            <SCRIPT language=Javascript>
            function clipboardLinkAdd(lnk, type)
            {
               if(type == 1)
               {
                  document.publishform.comment_content.value =                document.publishform.comment_content.value + "\n" + lnk;
               }
               else
               {
                  document.publishform.story_content.value =                document.publishform.story_content.value + "\n" + lnk;
               }
            }

            function clipboardLinkNotify()
            {
               alert("A html link tag to the item you've selected has been pasted to the end of the contents section of the publish form.");
            }
            function clipboardImageNotify()
            {
               alert("A html image tag of the image you've selected has been pasted to to the end of the contents section of the publish form.  You can edit the width field to display the image at a smaller scale");
            }
            </SCRIPT>
            <?
            if((isset($_REQUEST['section']) && $_REQUEST['section'] == 'addcomment') or (isset($_REQUEST['editcomment']))) $section = 1;
            else $section = 0;
            if($item_type=="attachment" && $item->isImage())
            {
               $size = getimagesize($system_config->attachment_store.$item->attachment_file);
               ?>
               <a onclick="JavaScript:clipboardLinkAdd('<img src=\'<?=$linkurl?>\' title=\'<?=addslashes($linkname)?>\' alt=\'<?=addslashes($linkname)?>\' width=<?=$size[0]?> align=right>', <?=$section?>);clipboardImageNotify();"><img title="Copy html image tag of <?=$item_type?> <?=$item_id?> to system clipboard" alt="Copy html image tag of <?=$item_type?> <?=$item_id?> to system clipboard" src='<?=$graphics_store?>copyimagetag.gif' border=0></a>
               <?
            }
            ?>
            <a onclick="JavaScript:clipboardLinkAdd('<a href=\'<?=$linkurl?>\'><?=addslashes($linkname)?></a>', <?=$section?>);clipboardLinkNotify();"><img title="Copy html link tag of <?=$item_type?> <?=$item_id?> to system clipboard" alt="Copy html link tag of <?=$item_type?> <?=$item_id?> to system clipboard" src="<?=$graphics_store?>copylinktag.gif" border=0></a>
            <?
         }
         return true;
      }
      else return false;
   }
}