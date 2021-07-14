from distutils.core import setup, Extension

module = Extension( 'u2py_internals',
                    include_dirs =  [   '../../../../include'   ],
                    library_dirs =  [   '../../../../_debug'    ],
                    libraries =     [   'U2Cored',
                                        'U2Langd',
                                        'U2Algorithmd',
                                        'U2Formatsd',
                                        'U2Remoted',
                                        'U2Privated',
                                        'U2Scriptd',
                                        'ugenedbd',
                                        'U2Guid'                ],
                    sources =       [   'u2py_internals.c'      ]
                    )

setup(  name =          'u2py_internals',
        version =       '0.1.0',
        description =   'API for UGENE Workflow Designer',
        license =       '../../../../../LICENSE.txt',
        author =        'Ivan Protsyuk',
        author_email =  'iprotsyuk@unipro.ru',
        url =           'http://ugene.net/download.html',
        ext_modules =   [ module ]
        )
