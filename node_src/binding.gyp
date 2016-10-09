{
  "targets": [
    {
      "target_name": "ctcp",
      "sources": [ "addon.cpp" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
