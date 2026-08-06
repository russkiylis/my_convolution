with inserted_conv as (
    insert into df_conv (
                         data_type,
                         min_angle_h,
                         max_angle_h,
                         step_h,
                         count_h,
                         min_angle_v,
                         max_angle_v,
                         step_v,
                         count_v,
                         quality,
                         conv)
        values (:data_type,
                :min_angle_h,
                :max_angle_h,
                :step_h,
                :count_h,
                :min_angle_v,
                :max_angle_v,
                :step_v,
                :count_v,
                :quality,
                :conv)
        returning id
)
insert into df_result (
                       id,
                       result_timestamp,
                       azimuth,
                       elevation,
                       power,
                       frequency,
                       longitude,
                       latitude,
                       sysname)
SELECT id,  -- Высовываем id из верхнего запроса
       :result_timestamp,
       :azimuth,
       :elevation,
       :power,
       :frequency,
       :longitude,
       :latitude,
       :sysname
FROM inserted_conv;

