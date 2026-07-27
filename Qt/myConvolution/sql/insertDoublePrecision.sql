insert into df_conv (
     data_type,
     min_angle_h,
     max_angle_h,
     step_h,
     conv_h_double,
     min_angle_v,
     max_angle_v,
     step_v,
     conv_v_double)
values ('double_precision',
        :min_angle_h,
        :max_angle_h,
        :step_h,
        CAST(:conv_h_double AS double precision[]),
        :min_angle_v,
        :max_angle_v,
        :step_v,
        CAST(:conv_v_double AS double precision[]));