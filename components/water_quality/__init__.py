import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_DATA
) 
CODEOWNERS = ["@hasatio"]
AUTO_LOAD = ["sensor"]
MULTI_CONF = True

CONF_X = "x"
CONF_Y = "y"
CONF_X1 = "x1"
CONF_Y1 = "y1"
CONF_X2 = "x2"
CONF_Y2 = "y2"
CONF_X3 = "x3"
CONF_Y3 = "y3"
CONF_X4 = "x4"
CONF_Y4 = "y4"
CONF_PUMP1 = "pump1"
CONF_PUMP2 = "pump2"
CONF_PUMP3 = "pump3"
CONF_PUMP4 = "pump4"
CONF_PUMP5 = "pump5"
CONF_PUMP6 = "pump6"
CONF_TYPE = "type"
CONF_PUMP_CALIBRATION = "pump_calibration"
CONF_PUMP_DOSE = "pump_dose"
CONF_PUMP_TOTAL = "pump_total"
CONF_PUMP_STATUS = "pump_status"
CONF_ANALOG_OUTPUT = "analog_output"

PUMP_TYPE_NULL = 0
PUMP_TYPE_DOSE = 1
PUMP_TYPE_CIRCULATION = 2
PUMP_TYPES_SUPPORTED = [PUMP_TYPE_NULL, PUMP_TYPE_DOSE, PUMP_TYPE_CIRCULATION]

UNIT_MILILITER = "ml"
UNIT_MILILITERS_PER_MINUTE = "ml/min"

component_ns = cg.esphome_ns.namespace("water_quality")
MyComponent = component_ns.class_("MyComponent", cg.Component)

PUMP_CALIBRATION_SCHEMA = cv.Schema(
    {
        # cv.GenerateID(): cv.use_id(MyComponent),
        cv.Required(CONF_PUMP_CALIBRATION): cv.All(
            cv.ensure_list(
                cv.Schema(
                    {
                        cv.Required(CONF_X): cv.All(
                            cv.ensure_list(cv.uint8_t),
                            cv.Length(min=8, max=8),
                        ),
                        cv.Required(CONF_Y): cv.All(
                            cv.ensure_list(cv.uint8_t),
                            cv.Length(min=8, max=8),
                        ),
                    }
                )
            )
        ),
    }
)
                    
PUMP_TYPE_SCHEMA = cv.typed_schema(
    {
        PUMP_TYPE_NULL: cv.Schema({}),
        PUMP_TYPE_DOSE: PUMP_CALIBRATION_SCHEMA.extend(
            {
                # cv.GenerateID(): cv.declare_id(MyComponent),
                
            }
        ).extend(cv.COMPONENT_SCHEMA),
        PUMP_TYPE_CIRCULATION: PUMP_CALIBRATION_SCHEMA.extend(
            {
                # cv.GenerateID(): cv.declare_id(MyComponent),
                
            }
        ).extend(cv.COMPONENT_SCHEMA),
    },
    # key=CONF_PUMP_TYPE,
    default_type=PUMP_TYPE_NULL,
    int=True,
)
                    
CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MyComponent),
            # cv.Required(CONF_PUMP_TYPE): cv.All(
            #                     cv.ensure_list(cv.int_range(min=0, max=2)),
            #                     cv.Length(min=6, max=6),
            # ),
        
    # .extend(cv.COMPONENT_SCHEMA),
            # cv.Required(CONF_PUMP_CALIBRATION): cv.All(
            #     cv.ensure_list(
            #         cv.Schema(
            #             {
            #                 cv.Required(CONF_X1): cv.All(
            #                     cv.ensure_list(cv.uint8_t),
            #                     cv.Length(min=8, max=8),
            #                 ),
            #                 cv.Required(CONF_Y1): cv.All(
            #                     cv.ensure_list(cv.uint8_t),
            #                     cv.Length(min=8, max=8),
            #                 ),
            #                 cv.Required(CONF_X2): cv.All(
            #                     cv.ensure_list(cv.uint8_t),
            #                     cv.Length(min=8, max=8),
            #                 ),
            #                 cv.Required(CONF_Y2): cv.All(
            #                     cv.ensure_list(cv.uint8_t),
            #                     cv.Length(min=8, max=8),
            #                 ),
            #                 cv.Required(CONF_X3): cv.All(
            #                     cv.ensure_list(cv.uint8_t),
            #                     cv.Length(min=8, max=8),
            #                 ),
            #                 cv.Required(CONF_Y3): cv.All(
            #                     cv.ensure_list(cv.uint8_t),
            #                     cv.Length(min=8, max=8),
            #                 ),
            #                 cv.Required(CONF_X4): cv.All(
            #                     cv.ensure_list(cv.uint8_t),
            #                     cv.Length(min=8, max=8),
            #                 ),
            #                 cv.Required(CONF_Y4): cv.All(
            #                     cv.ensure_list(cv.uint8_t),
            #                     cv.Length(min=8, max=8),
            #                 ),
            #             }
            #         ).extend(cv.COMPONENT_SCHEMA),
            #     ),
            #     cv.Length(max=12),
            # ),
    # cv.typed_schema(
    #     {
    #         # PUMP_TYPE_NULL: cv.Schema(),
    #         PUMP_TYPE_DOSE: CALIBRATION_SCHEMA.extend(
    #             {
    #                 # cv.GenerateID(): cv.declare_id(MyComponent),
                    
    #             }
    #         ).extend(cv.COMPONENT_SCHEMA),
    #         PUMP_TYPE_CIRCULATION: CALIBRATION_SCHEMA.extend(
    #             {
    #                 # cv.GenerateID(): cv.declare_id(MyComponent),
                    
    #             }
    #         ).extend(cv.COMPONENT_SCHEMA),
    #     },
    #     key=CONF_PUMP_TYPE1,
    #     default_type=PUMP_TYPE_NULL,
    #     int=True,
    # )           
    # PUMP_TYPE_SCHEMA,
            cv.Required(CONF_PUMP1): cv.All(
                PUMP_CALIBRATION_SCHEMA, cv.Length(min=1)
            ),
            # if config[CONF_PUMP_TYPE][0] == 1:
            #     cv.Required(CONF_DATA): cv.All(
            #         cv.ensure_list(cv.uint8_t),
            # ),
            cv.Required(CONF_PUMP2): cv.All(
                cv.ensure_list(PUMP_TYPE_SCHEMA), cv.Length(min=1)
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    # if CONF_PUMP_TYPE in config:
    #     conf = config[CONF_PUMP_TYPE]
    #     cg.add(var.pump_type(conf))
    # if CONF_PUMP_CALIBRATION in config:
    #     for conf in config[CONF_PUMP_CALIBRATION]:
    #         cg.add(var.pump_calibration(
    #             conf[CONF_X1], 
    #             conf[CONF_Y1],
    #             conf[CONF_X2], 
    #             conf[CONF_Y2],
    #             conf[CONF_X3], 
    #             conf[CONF_Y3],
    #             conf[CONF_X4], 
    #             conf[CONF_Y4],
    #             ))

    # if config[CONF_PUMP_TYPE][0] == 0:
    #     cg.add(var.test(212))
    
    
    # if config[CONF_PUMP1] != 0:
    con = config[CONF_PUMP1]
    for conf in con[CONF_PUMP_CALIBRATION]:
        # for conf in con[CONF_PUMP_CALIBRATION]:
        arr = [conf[CONF_X], conf[CONF_Y]]
        cg.add(var.pump_calibration(arr))
            
    if config[CONF_PUMP2][0] != PUMP_TYPE_NULL:
        con = config[CONF_PUMP2][0]
        # for conf in con[CONF_PUMP_CALIBRATION]:
        #     cg.add(var.pump_type(conf[CONF_X]))
        cg.add(var.pump_type(con[CONF_PUMP_CALIBRATION][0]))
            

# PumpTypeAction = component_ns.class_("PumpTypeAction", automation.Action)
PumpDoseAction = component_ns.class_("PumpDoseAction", automation.Action)


# PUMP_TYPE_ACTION_SCHEMA = cv.All(
#     {
#         cv.GenerateID(): cv.use_id(MyComponent),
#         cv.Required(CONF_PUMP_TYPE): cv.All(
#                 [cv.Any(cv.uint8_t)],
#                 cv.Length(min=6, max=6),
#         ),
#     }
# )

PUMP_DOSE_ACTION_SCHEMA = cv.All(
    {
        # cv.Required(CONF_ID): cv.use_id(MyComponent),
        cv.GenerateID(): cv.use_id(MyComponent),
        # cv.Required(CONF_DOSE): cv.templatable(
        #     cv.int_range()
        # ),
        cv.Required(CONF_PUMP_DOSE): cv.All(
                [cv.Any(cv.uint8_t)],
        ),
    }
)


# @automation.register_action(
#     "water_quality.pump_type", 
#     PumpTypeAction, 
#     PUMP_TYPE_ACTION_SCHEMA
# )

@automation.register_action(
    "water_quality.pump_dose", 
    PumpDoseAction, 
    PUMP_DOSE_ACTION_SCHEMA
)


# async def pump_type_to_code(config, action_id, template_arg, args):
#     paren = await cg.get_variable(config[CONF_ID])
#     var = cg.new_Pvariable(action_id, template_arg, paren)

#     type = config[CONF_PUMP_TYPE]
#     cg.add(var.set_type(type))

#     return var

async def pump_dose_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)

    dose = config[CONF_PUMP_DOSE]
    # template_ = await cg.templatable(dose, args, cg.uint8)
    # cg.add(var.set_data(template_))
    cg.add(var.set_dose(dose))

    return var
