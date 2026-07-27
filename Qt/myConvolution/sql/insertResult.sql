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
values (
        :id,
        :result_timestamp,
        :azimuth,
        :elevation,
        :power,
        :frequency,
        :longitude,
        :latitude,
        :sysname
       );