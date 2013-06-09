{
  'targets': [
    {
      'target_name': 'redisLibuvAdapter',
      'type': 'static_library',

      'sources': [
        'redisLibuvAdapter.c'
      ],

      'include_dirs': [
        '.',
        '../hiredis/',
        '../libuv/include/'
      ]
    }
  ]
}
