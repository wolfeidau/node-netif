{
    "targets":[
        {
            "target_name":"netif",
            "sources":[
                "src/netif.cc"
            ],
            "include_dirs":[
                "<!(node -e \"require('nan')\")"
            ]
        }
    ]
}