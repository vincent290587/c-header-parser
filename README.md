# c-header-parser

This program can parse C header files to extract the struct/union/enum definitions, and with these definitions to analyse the memory dump data of the struct/union. The analysis result can be printed in a nice format with both the stuct/union member names and their values.

## YAML generation

```yaml
# TTC Group time ICD

type: TTC
name: tc_group_time
version: 0.1


enums:
    ttc_radioselectioncmd_t:
        datatype: UINT8
        values: {radio_TRXVU: 0, radio_TXS: 1}
    ttc_subcmd_spacecrafttime_t:
        datatype: UINT8
        values: {subcmd_spacecrafttime_none: 0, subcmd_spacecrafttime_set: 1, subcmd_spacecrafttime_setdelayed: 2}

structs:
    ttc_sctimesetcmd_t:
        description: Set time command
        parameters:
            - radio_id:
                description: Radio selection
                data:
                    type: ttc_radioselectioncmd_t
            - new_epoch:
                description: New epoch used or setting time
                data:
                    type: UINT32

```