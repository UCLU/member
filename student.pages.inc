<?php
/**
 * @file
 * Menu callbacks for pages in the Student module.
 */

/**
 * Menu callback for a student, path: student/%student.
 *
 * @param stdClass $student
 * @param string $view_mode
 *
 * @return array
 *   Render array.
 */
function student_page_view($student, $view_mode = 'full') {

  if ($view_mode == 'full') {
    drupal_set_title(entity_label('student', $student));
    $uri = entity_uri('student', $student);
    drupal_add_html_head_link(array('rel' => 'canonical', 'href' => url($uri['path'], $uri['options'])), TRUE);
    drupal_add_html_head_link(array('rel' => 'shortlink', 'href' => url($uri['path'], array_merge($uri['options'], array('alias' => TRUE)))), TRUE);
  }

  return student_view($student, $view_mode);

}