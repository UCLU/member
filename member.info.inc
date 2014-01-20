<?php
/*
 * @file
 * Entity property information for the Member module.
 */

/**
 * Implements hook_entity_property_info().
 */
function member_entity_property_info() {
  $info = array();
  $properties = &$info['member']['properties'];
  $properties['member_id'] = array(
    'label' => t('Member ID'),
    'type' => 'integer',
    'description' => t('The unique ID of the member entity.'),
    'schema field' => 'member_id',
    'required' => TRUE,
  );
  $properties['name'] = array(
    'label' => t('Name'),
    'type' => 'text',
    'description' => t('The full name of the member.'),
    'schema field' => 'name',
  );
  $properties['status'] = array(
    'label' => t('Membership status'),
    'type' => 'integer',
    'description' => t('The status of the member.'),
    'options list' => 'member_get_status_options',
    'schema field' => 'status',
  );
  $properties['type'] = array(
    'label' => t('Type'),
    'type' => 'token',
    'description' => t('The type of member (machine-readable name).'),
    'required' => TRUE,
    'schema field' => 'type',
  );
  $properties['uid'] = array(
    'label' => t('User ID'),
    'type' => 'user',
    'description' => t('The Drupal user ID associated with the member.'),
    'schema field' => 'uid',
  );
  $properties['uuid'] = array(
    'label' => t('UUID'),
    'type' => 'text',
    'description' => t('The universally unique identifier of the member.'),
    'schema field' => 'uuid',
    'required' => TRUE,
  );
  $properties['created'] = array(
    'label' => t('Created date'),
    'type' => 'date',
    'description' => t('The date and time when the member entity was created.'),
    'schema field' => 'created',
  );
  $properties['changed'] = array(
    'label' => t('Changed date'),
    'type' => 'date',
    'description' => t('The date and time when the member entity was last modified.'),
    'schema field' => 'changed',
  );
  return $info;
}
