<?php
/**
 * @file
 * Menu callbacks for pages in the Member module.
 */

/**
 * Menu callback for a member, path: member/%member.
 *
 * @param stdClass $member
 * @param string $view_mode
 *
 * @return array
 *   Render array.
 */
function member_page_view($member, $view_mode = 'full') {

  if ($view_mode == 'full') {
    drupal_set_title(entity_label('member', $member));
    $uri = entity_uri('member', $member);
    drupal_add_html_head_link(array('rel' => 'canonical', 'href' => url($uri['path'], $uri['options'])), TRUE);
    drupal_add_html_head_link(array('rel' => 'shortlink', 'href' => url($uri['path'], array_merge($uri['options'], array('alias' => TRUE)))), TRUE);
  }

  return member_view($member, $view_mode);

}