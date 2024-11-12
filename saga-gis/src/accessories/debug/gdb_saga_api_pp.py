# Note on my machine the gnu pretty-printers are in this file /usr/share/gcc-xx.x.x/python/libstdcxx/printers.py

import gdb
import datetime

class array_printer:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        address = int(self.val['m_Values'])
        count = int(self.val['m_nValues'])
        buffer = int(self.val['m_nBuffer'])
        element_size = int(self.val['m_Value_Size'])

        data = gdb.selected_inferior().read_memory(address, count * element_size)
        
        # Hier müssen Sie die Daten basierend auf der Größe jedes Elements interpretieren
        data_str = ''
        for i in range(0, len(data), element_size):
            data_str += data[i:i+element_size].hex()  # ))#, 'little')) 
            if(i+element_size != len(data)):
                data_str += ', '

        return 'CSG_Array: {size = ' + str(count) + ', buffer = ' + str(buffer) + ', values = {' + data_str + '}}'


    def dispay_hint(self):
        return 'array'

class array_int_printer:
    def __init__(self, val):
        self.val = val
        self.address = int(self.val['m_Array']['m_Values'])
        self.count = int(self.val['m_Array']['m_nValues'])
        self.buffer = int(self.val['m_Array']['m_nBuffer'])
        self.element_size = int(self.val['m_Array']['m_Value_Size'])

    def values(self):
        int_t = gdb.lookup_type('int').pointer()
        values = ''
        for i in range(self.count):
            element = gdb.Value(self.address + i*self.element_size ).cast(int_t).dereference()
            values += str(element)
            if( i+1 != self.count ):
                values += ', '

        return values

    def to_string(self):
        return 'CSG_Array_Int: {size = ' + str(self.count) + ', buffer = ' + str(self.buffer) + ', values = {' + self.values() + '}}'

    def dispay_hint(self):
        return 'array'

class array_pointer_printer:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        address = int(self.val['m_Array']['m_Values'])
        count = int(self.val['m_Array']['m_nValues'])
        buffer = int(self.val['m_Array']['m_nBuffer'])
        element_size = int(self.val['m_Array']['m_Value_Size'])

        data = gdb.selected_inferior().read_memory(address, count * element_size)
        
        # Hier müssen Sie die Daten basierend auf der Größe jedes Elements interpretieren
        data_str = ''
        for i in range(0, len(data), element_size):
            data_str += str(hex(int.from_bytes(data[i:i+element_size], 'little')))  # ))#, 'little')) 
            #data_str += str(read_data(address, count, element_size)  # ))#, 'little')) 
            if(i+element_size != len(data)):
                data_str += ', '

        return 'CSG_Array_Pointer: {size = ' + str(count) + ', buffer = ' + str(buffer) + ', values = {' + data_str + '}}'


    def dispay_hint(self):
        return 'array'

class data_object_printer:
    def __init__(self, val):
        self.val = val

    def format(self):
        name = str(self.val['m_Name'])
        file_name = str(self.val['m_FileName'])
        nodata_value_lo = str(self.val['m_NoData_Value'][0])
        nodata_value_hi = str(self.val['m_NoData_Value'][1])
        modified = str(self.val['m_bModified'])
        ref_id = str(self.val['m_RefID'])
        managed = str(self.val['m_Managed'])
        max_samples = str(self.val['m_Max_Samples'])

        projection = projection_printer(self.val['m_Projection']).format()

        return 'name = ' + name + ', file name: ' + file_name + ', nodata value ={' + nodata_value_lo + ', ' + nodata_value_hi + '}, max saples: ' + max_samples + ', projection ={'+ projection + '} ref id = ' + ref_id + ', managed = ' + managed + ', medified = ' + modified  

    def to_string(self):
        return 'CSG_Data_Object: {' + self.format() + '}'

class datetime_printer:
    def __init__(self, val):
        self.val = val
        self.time = self.val['m_pDateTime']['m_time']['m_ll']

    def value(self):
        time = str(self.time)[:10]
        try:
            dt = datetime.datetime.fromtimestamp(int(time),datetime.timezone.utc)
            time = '{:%Y-%m-%d %H:%M:%S}'.format(dt)
        except:
            pass
        return time

    def to_string(self):
        return 'CSG_DateTime: iso 8601 = ' + self.value() + ', unix = ' + str(self.time)

class grid_printer:
    def __init__(self, val):
        self.val = val
        self.offset = self.val['m_zOffset']
        self.scale = self.val['m_zScale']
        self.data_type = self.val['m_Type'] # = SG_DATATYPE_Float, 
        self.unit = self.val['m_Unit'] 
        self.stat = self.val['m_Statistics']
        self.system = self.val['m_System']

    def format(self):
        return "data type = %s, unit = %s, scale = %f, offset = %f, statistic ={%s} "  % ( str(self.data_type)[12:], str(self.unit), self.scale, self.offset, simple_statistics_printer(self.stat).format() )

    def to_string(self):
        return "CSG_Grid: {%s, grid system ={%s}, data object ={%s}}" % ( self.format(), grid_system_printer(self.system).format(), data_object_printer(self.val).format() )

class grid_system_printer:
    def __init__(self, val):
        self.val = val

    def format(self):
        columns = int(self.val['m_NX'])
        rows = int(self.val['m_NY'])
        count = int(self.val['m_NCells'])
        size = float(self.val['m_Cellsize'])
        area = float(self.val['m_Cellarea'])
        extent = self.val['m_Extent']
        return "columns = %d, rows = %d, count = %d, cellsize = %f, cellarea = %f, extent = {%s}" % ( columns, rows, count, size, area, rect_printer(extent).format() )

    def to_string(self):
        return 'CSG_Grid_System: {' + self.format() + '}'


class parameter_printer:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        ident = str(self.val['m_Identifier'])
        name = str(self.val['m_Name'])
        type = self.val.dynamic_type 
        type_string = str(type)[14:]
        description = str(self.val['m_Description'])
        enabled = str(self.val['m_bEnabled'])
        parameters = str(self.val['m_pParameters'])
        parent = str(self.val['m_pParent'])
        nchild = str(self.val['m_nChildren'])
        children = str(self.val['m_Children'])


            # value_string += str( polymorph['m_pMin'].dereference() )
        
        return 'CSG_Parameter = Identifier = ' + ident + ', Name = ' + name + ', Description = ' + description + ', Enabled = ' + enabled +', Type = {' + self.value() + '}, Parameters = ' + parameters + ', Parent = ' + parent + ', Children Count = ' + nchild + ', Children = ' + children

    def value(self):
        default = str(self.val['m_Default'])
        type = self.val.dynamic_type 
        type_string = str(type)[14:]
        polymorph = self.val.cast(type)

        value_string = ''
        if( type_string == "Node" ):
            value_string = 'Node'

        if( type_string == "Bool" ):
            value_string = 'Bool = ' + str(polymorph['m_Value'])


        if( type_string == "Double" or type_string == "Int"):
            value_string = type_string + ' = ' + str(polymorph['m_Value'])
            if( bool(polymorph['m_bMinimum']) == True ):
                value_string += ', Minimum = ' + str(polymorph['m_Minimum'])
            else:
                value_string += ', Minimum = ' + str(polymorph['m_bMinimum'])
            if( bool(polymorph['m_bMaximum']) == True ):
                value_string += ', Maximum = ' + str(polymorph['m_Maximum'])
            else:
                value_string += ', Maximum = ' + str(polymorph['m_bMaximum'])

            #value_string += str(default)
        
        if( type_string == "Range" ):

            range_type = polymorph['m_pMin'].dynamic_type 
            min_val = polymorph['m_pMin'].cast(range_type)
            max_val = polymorph['m_pMax'].cast(range_type)

            value_string = 'Range = {' + str(min_val['m_Value']) + ', ' + str(max_val['m_Value']) + '}'

            if( bool(min_val['m_bMinimum']) == True ):
                value_string += ', Minimum = ' + str(min_val['m_Minimum'])
            else:
                value_string += ', Minimum = ' + str(min_val['m_bMinimum'])
            if( bool(min_val['m_bMaximum']) == True ):
                value_string += ', Maximum = ' + str(min_val['m_Maximum'])
            else:
                value_string += ', Maximum = ' + str(min_val['m_bMaximum'])

        if( type_string == "Choice" ):
            value_string = 'Choice = ' + str(polymorph['m_Value']) + ', Items = {' + strings_printer(polymorph['m_Items']).values() + '}'

        if( type_string == "Choices" ):
            selection = str(array_int_printer(polymorph['m_Selection']).values())
            items = str(strings_printer(polymorph['m_Items'][0]).values())
            data = str(strings_printer(polymorph['m_Items'][1]).values())
            value_string = 'Choices ={' + selection + '}, Items ={' + items + '}, Data ={' + data + '}'

        if( type_string == "String" ):
            value_string = 'String = ' + string_printer(polymorph['m_String']).to_string() + ', Default = ' + default 

        if( type_string == "Date" ):
            value_string = 'Date = ' + datetime_printer(polymorph['m_Date']).value() + ', Default = ' + default 

        return value_string

class parameters_printer:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        return 'Identifier = ' + str(self.val['m_Identifier'])

class projection_printer:
    def __init__(self, val):
        self.val = val

    def format(self):
        name = str(self.val['m_Name'])
        authority_name = str(self.val['m_Authority'])
        authority_id = str(self.val['m_Authority_ID'])
        projection_type = str(self.val['m_Type'])
        projection_type = str(projection_type[16:])
        projection_unit = str(self.val['m_Unit'])
        projection_unit = str(projection_unit[13:])
        proj_string = str(self.val['m_Proj4'])
        wkt_string = str(self.val['m_WKT'])

        return 'name = ' + name + ', authority = {' + authority_name + ',  ' + authority_id + '}, type = ' + projection_type + ', unit = ' + projection_unit + ', proj = ' + proj_string + ', wkt = ' + wkt_string

    def to_string(self):
        return 'CSG_Projection: {' + self.format() + '}'

class rect_printer:
    def __init__(self, val):
        self.val = val

    def format(self):
        xmin = self.val['xMin']
        ymin = self.val['yMin']
        xmax = self.val['xMax']
        ymax = self.val['yMax']
        return "left = %f, right = %f, up = %f, down = %f" % ( xmin, xmax, ymax, ymin )

    def to_string(self):
        return "CSG_Rect: {%s}" % ( self.format() )

class shape_printer:
    def __init__(self, val):
        self.val = val

        #self.val = val.cast(gdb.lookup_type('CSG_Shape_Points'))
    def to_string(self):
        obj_type = self.val.dynamic_type 
        cast = self.val.cast(obj_type)

        if( obj_type == gdb.lookup_type('CSG_Shape_Point') ):
            return shape_point_printer(cast).to_string()
            #return "Printer"

        if( obj_type == gdb.lookup_type('CSG_Shape_Point').pointer() ):
            return shape_point_printer(cast).to_string()

        if( obj_type == gdb.lookup_type('CSG_Shape_Points') ):
            return shape_points_printer(cast).to_string()
            #return "Printer"

        if( obj_type == gdb.lookup_type('CSG_Shape_Points').pointer() ):
            return shape_points_printer(cast).to_string()
            #return "Printer"
        return "shape_printer successfully " + str(obj_type)

class shape_part_printer:
    def __init__(self, val):
        self.val = val
        self.count = int(self.val['m_nPoints'])
        self.points = self.val['m_Points']
        self.zvals = self.val['m_Z']
        self.mvals = self.val['m_M']

    def to_string(self):
        # shape_part_p_t = gdb.lookup_type('CSG_Shape_Part').pointer()
        #string_p_t = gdb.lookup_type('CSG_String').pointer()
        # string_ptr = gdb.Value(self.parts + i*shape_part_p_t.sizeof ).reinterpret_cast(void_pp_t).dereference().reinterpret_cast(string_p_t)
        tsg_point_p_t = gdb.lookup_type('TSG_Point').pointer()

        values = ''
        #string_p_t = gdb.lookup_type('CSG_String').pointer()
        # string_ptr = gdb.Value(self.parts + i*shape_part_p_t.sizeof ).reinterpret_cast(void_pp_t).dereference().reinterpret_cast(string_p_t)

        values += self.val.type.name
        #values += str(self.parts)
        for i in range(self.count):

            values += 'test'
            #point = gdb.Value(self.points + i*tsg_point_p_t.sizeof ).dereference()
            #values += str(point['x']) + ', ' + str(point['y'])
        #values += str(self.parts)

        return str(self.points.type) + str(self.count)

        for i in range(self.count):
            part_ptr = gdb.Value(self.parts + i*shape_part_p_t.sizeof ).cast(shape_part_p_t)
            values += str( part_ptr )
            values += str( part_ptr.type )
        return "shape_part_printer successfully "

"""
{_vptr.CSG_Shape_Part = 0x7ffff6e0c968 <vtable for CSG_Shape_Part+16>, 
    m_bUpdate = false, 
    m_nPoints = 28, 
    m_nBuffer = 28, 
    m_Z = 0x55555eef3520, 
    m_ZMin = 0, 
    m_ZMax = 14.666666666666666, 
    m_M = 0x55555ef5bfd0, 
    m_MMin = 0,
    m_MMax = 14.66666666666667, 
    m_Points = 0x55555f3318c0, 
    m_Extent = {<SSG_Rect> = {xMin = 0.35616099197477324, yMin = 0.61964419765712786, xMax = 0.72878122881547802, yMax = 0.92463353636269607}, <No data fields>},
    m_pOwner = 0x55555e653720}
"""

class shape_point_printer:
    def __init__(self, val):
        self.val = val
        self.x = self.val['m_Point']['x']
        self.y = self.val['m_Point']['y']

    def to_string(self):
        return "CSG_Shape_Point: {point ={x = %f, y = %f}, table record ={%s}" % ( self.x, self.y, table_record_printer(self.val).format() )

class shape_point_z_printer:
    def __init__(self, val):
        self.val = val
        self.x = self.val['m_Point']['x']
        self.y = self.val['m_Point']['y']
        self.z = self.val['m_Z']

    def to_string(self):
        return "CSG_Shape_Point_Z: {point ={x = %f, y = %f, z = %f}, table record ={%s}" % ( self.x, self.y, self.z, table_record_printer(self.val).format() )

class shape_point_zm_printer:
    def __init__(self, val):
        self.val = val
        self.x = self.val['m_Point']['x']
        self.y = self.val['m_Point']['y']
        self.z = self.val['m_Z']
        self.m = self.val['m_M']

    def to_string(self):
        return "CSG_Shape_Point_ZM: {point ={x = %f, y = %f, z = %f, m = %f}, table record ={%s}" % ( self.x, self.y, self.z, self.m, table_record_printer(self.val).format() )

class shape_points_printer:
    def __init__(self, val):
        self.val = val
        self.part_count = int(self.val['m_nParts'])
        self.parts = self.val['m_pParts']
        self.points = int(self.val['m_nPoints'])
        self.zmin = float(self.val['m_ZMin'])
        self.zmax = float(self.val['m_ZMax'])
        self.mmin = float(self.val['m_MMin'])
        self.mmax = float(self.val['m_MMax'])
        self.extent = self.val['m_Extent']
        self.table = self.val['m_pTable']
        #self.update = self.val['m_bUpdate']

    def to_string(self):

        shape_part_p_t = gdb.lookup_type('CSG_Shape_Part').pointer()
        #string_p_t = gdb.lookup_type('CSG_String').pointer()
        # string_ptr = gdb.Value(self.parts + i*shape_part_p_t.sizeof ).reinterpret_cast(void_pp_t).dereference().reinterpret_cast(string_p_t)

        values = ''
        #values += str(self.parts)
        for i in range(self.part_count):
            part_ptr = gdb.Value(self.parts + i*shape_part_p_t.sizeof ).cast(shape_part_p_t)
            values = shape_part_printer(part_ptr).to_string()
            #values += str( part_ptr )
            #values += str( part_ptr.type )
            #values += str(string_printer(string_ptr.dereference()).to_string())
            #if( i+1 < self.count ):
             #   values += ', '

        return values

        obj_type = gdb.lookup_type('CSG_Shapes').pointer()
        vertex_type = str(self.table.cast(obj_type)['m_Vertex_Type'])[15:]

        optional_zm = ''
        if( vertex_type == 'XYZ' or vertex_type == 'XYZM' ):
            optional_zm += ", z min = %f, z max = %f" % ( self.zmin, self.zmax )
        if( vertex_type == 'XYZM' ):
            optional_zm += ", m min = %f, m max = %f" % ( self.mmin, self.mmax )

        return "CSG_Shape_Points: {parts = %d, points = %d, exten ={%s}%s, table record ={%s}, parts ={%s}}" % ( self.part_count, self.points, rect_printer(self.extent).format(), optional_zm, table_record_printer(self.val).format() )

    """
CSG_Shape_Points: {
    parts = 3, points = 3, 
    exten ={
        left = 0.356161, right = 0.728781, up = 0.924634, down = 0.619644
    }, 
    z min = 0.000000, z max = 0.329412, m min = 0.000000, m max = 0.000000, table 
    record ={index = 0, 
        fields = 2 {
            1, "One"
        }
    }
}}
{<CSG_Shape> = {
    <CSG_Table_Record> = {_vptr.CSG_Table_Record = 0x7ffff6e0cb88 <vtable for CSG_Shape_Points+16>, 
        m_Flags = 0 '\000', 
        m_Index = 0, 
        m_Values = 0x55555f4ab810, 
        m_pTable = 0x55555e8212b0
    },
    <No data fields>
},
m_bUpdate = false, 
m_nParts = 3, 
m_nPoints = 28, 
m_ZMin = 8.6916947597937554e-311, 
m_ZMax = 0.32941176470588235, 
m_MMin = 0, 
m_MMax = 1.6296927674613291e-310, 
m_Extent = {<SSG_Rect> = {xMin = 0.35616099197477324,
      yMin = 0.61964419765712786, xMax = 0.72878122881547802, yMax = 0.92463353636269607}, <No data fields>}, m_pParts = 0x55555f158700}
    """

class shapes_printer:
    def __init__(self, val):
        self.val = val

    def format(self):
        shapes_type = self.val['m_Type']
        shapes_type_string = str(shapes_type)[11:]
        vertex_type = self.val['m_Vertex_Type']
        vertex_type_string = str(vertex_type)[15:]
        zmin = int(self.val['m_ZMin'])
        zmax = int(self.val['m_ZMax'])
        mmin = int(self.val['m_ZMin'])
        mmax = int(self.val['m_ZMax'])
        s = "shape type = %s, vertex type = %s" % (shapes_type_string, vertex_type_string )
        if( vertex_type_string == 'XYZ' or vertex_type_string == 'XYZM' ):
            s += ", z min = %f, z max = %f" % ( zmin, zmax ) 
        if( vertex_type_string == 'XYZM' ):
            s += ", m min = %f, m max = %f" % ( mmin, mmax ) 
        return s

    def to_string(self):
        return "CSG_Shapes: {%s, table ={%s}, data object ={%s}}" % ( self.format(), table_printer(self.val).format(), data_object_printer(self.val).format() )

class simple_statistics_printer:
    def __init__(self, val):
        self.val = val
        self.count = self.val['m_nValues']
        self.sorted = bool(self.val['m_bSorted'])
        self.eval_level = int(self.val['m_bEvaluated'])
        self.size = int(self.val['m_nValues'])
        self.weights = float(self.val['m_Weights'])
        self.sum = float(self.val['m_Sum'])
        #self.sum2 = float(self.val['m_Sum2'])
        self.min = float(self.val['m_Minimum'])
        self.max = float(self.val['m_Maximum'])
        self.range = float(self.val['m_Range'])
        self.mean = float(self.val['m_Mean'])
        self.variance = float(self.val['m_Variance'])
        self.std_dev = float(self.val['m_StdDev'])
        self.kurtosis = float(self.val['m_Kurtosis'])
        self.skewness = float(self.val['m_Skewness'])
        self.gini = float(self.val['m_Gini'])

        self.hold = bool(self.val['m_Values']['m_nValues'])

    def format(self):
        return "hold values = %s, sorted = %s, evaluation level = %d, size = %d, weights = %f, sum = %f, minimum = %f, maximum = %f, range = %f, mean = %f, variance = %f, standard deviation = %f, kurtosis = %f, skewness = %f, gini = %f" % (str(self.hold), str(self.sorted), self.eval_level, self.size, self.weights, self.sum, self.min, self.max, self.range, self.mean, self.variance, self.std_dev, self.kurtosis, self.skewness, self.gini)

    def to_string(self):
        return "CSG_Simple_Statistics: {%s}" % (self.format() )

class string_printer:
    def __init__(self, val):
        self.val = val
        self.string = str(self.val['m_pString']['m_impl'])

    def to_string(self):
        return str(self.string[1:]) 

class strings_printer:
    def __init__(self, val):
        self.val = val
        self.address = self.val['m_Strings']['m_Array']['m_Values']
        self.size = int(self.val['m_Strings']['m_Array']['m_Value_Size'])
        self.count = int(self.val['m_Strings']['m_Array']['m_nValues'])

    def values(self):
        void_pp_t = gdb.lookup_type('void').pointer().pointer()
        string_p_t = gdb.lookup_type('CSG_String').pointer()

        values = ''
        for i in range(self.count):
            string_ptr = gdb.Value(self.address + i*self.size ).reinterpret_cast(void_pp_t).dereference().reinterpret_cast(string_p_t)
            values += str(string_printer(string_ptr.dereference()).to_string())
            if( i+1 < self.count ):
                values += ', '

        return values

    def to_string(self):

        return "CSG_Strings: {size = %d, strings = {%s}}" % ( self.count, self.values() )

class table_printer:
    def __init__(self, val):
        self.val = val

    def format(self):

        n_fields = int(self.val['m_nFields'])
        n_records = int(self.val['m_nRecords'])
       
        fields = ''
        for i in range(n_fields):
            field_type = self.val['m_Field_Type'][i]
            field_type_string = str(field_type)[12:]
            field_name = self.val['m_Field_Name'][i]
            field_name_string = string_printer(field_name).to_string()
            fields += field_name_string + ' ' + field_type_string 
            if( i+1 != n_fields ):
                fields += ', '

        return "records = %d, fields = %d {%s}" % ( n_records, n_fields, fields )

    def to_string(self):
        return "CSG_Table: {%s, data object = {%s}}" % ( self.format(), data_object_printer(self.val).format() )


class table_record_printer:
    def __init__(self, val):
        self.val = val
        self.index = int(self.val['m_Index'])
        self.fields = int(self.val['m_pTable']['m_nFields'])

    def format(self):
        values = ""
        for i in range(self.fields):
            value = self.val['m_Values'][i]
            obj_type = gdb.lookup_type(str(value.dereference().dynamic_type))
            values += str(value.dereference().cast(obj_type)['m_Value'])
            if( i+1 != self.fields ):
                values += ', '

        return "index = %d, fields = %d {%s}}" % ( self.index, self.fields, values )


    def to_string(self):
        obj_type = self.val.type 

        if( obj_type == gdb.lookup_type('CSG_Shape_Point') ):
            return shape_point_printer(self.val).to_string()

        if( obj_type == gdb.lookup_type('CSG_Shape_Point').pointer() ):
            return shape_point_printer(self.val).to_string()

        if( obj_type == gdb.lookup_type('CSG_Shape_Point_Z') ):
            return shape_point_z_printer(self).to_string()

        if( obj_type == gdb.lookup_type('CSG_Shape_Point_Z').pointer() ):
            return shape_point_z_printer(self).to_string()

        if( obj_type == gdb.lookup_type('CSG_Shape_Point_ZM') ):
            return shape_point_zm_printer(self).to_string()

        if( obj_type == gdb.lookup_type('CSG_Shape_Point_ZM').pointer() ):
            return shape_point_zm_printer(self).to_string()

        if( obj_type == gdb.lookup_type('CSG_Shape_Points') ):
            return shape_point_printer(self.val).to_string()
            #return "Printer"

        if( obj_type == gdb.lookup_type('CSG_Shape_Points').pointer() ):
            return shape_point_printer(self.val).to_string()
            #return "Printer"

        return "CSG_Table_Record: {%s}" % self.format()


class meta_data_printer:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        name = str(self.val['m_Name'])
        content = str(self.val['m_Content'])
        parent = str(self.val['m_pParent'])
        dummy = str(self.val['m_pDummy'])
        children = int(self.val['m_Children']['m_nValues'])
        
        properties_names = str(strings_printer(self.val['m_Prop_Names']).values())
        properties_values = str(strings_printer(self.val['m_Prop_Values']).values())

        return "CSG_MetaData: {name = %s, content = %s, properties ={names ={%s}, values ={%s}}, parent = %s, dummy = %s, children = %d}" % ( name, content, properties_names, properties_values, parent, dummy, children )

def my_pp_func(val):
    if str(val.type) == 'CSG_Array': return array_printer(val)
    if str(val.type) == 'CSG_Array_Int': return array_int_printer(val)
    if str(val.type) == 'CSG_Array_Pointer': return array_pointer_printer(val)
    if str(val.type) == 'CSG_Parameter': return parameter_printer(val)
    #if str(val.type) == 'CSG_Parameters': return parameters_printer(val)
    if str(val.type) == 'CSG_String': return string_printer(val)
    if str(val.type) == 'CSG_String*': return string_printer(val)
    if str(val.type) == 'CSG_Strings': return strings_printer(val)
    if str(val.type) == 'CSG_Data_Object': return data_object_printer(val)
    if str(val.type) == 'CSG_Table': return table_printer(val)
    if str(val.type) == 'CSG_Projection': return projection_printer(val)
    if str(val.type) == 'CSG_Grid': return grid_printer(val)
    if str(val.type) == 'CSG_Grid_System': return grid_system_printer(val)
    if str(val.type) == 'CSG_MetaData': return meta_data_printer(val)
    if str(val.type) == 'CSG_DateTime': return datetime_printer(val)
    if str(val.type) == 'CSG_Rect': return rect_printer(val)
    if str(val.type) == 'CSG_Table_Record': return table_record_printer(val)
    if str(val.type) == 'CSG_Table_Record *': return table_record_printer(val)
    if str(val.type) == 'CSG_Shape': return shape_printer(val)
    if str(val.type) == 'CSG_Shape *': return shape_printer(val)
    if str(val.type) == 'CSG_Shape_Point': return shape_point_printer(val)
    if str(val.type) == 'CSG_Shape_Point *': return shape_point_printer(val)
    if str(val.type) == 'CSG_Shape_Points': return shape_points_printer(val)
    if str(val.type) == 'CSG_Shape_Points *': return shape_points_printer(val)
    if str(val.type) == 'CSG_Shape_Point_Z': return shape_point_z_printer(val)
    if str(val.type) == 'CSG_Shape_Point_Z *': return shape_point_z_printer(val)
    if str(val.type) == 'CSG_Shape_Point_ZM': return shape_point_zm_printer(val)
    if str(val.type) == 'CSG_Shape_Point_ZM *': return shape_point_zm_printer(val)
    if str(val.type) == 'CSG_Shapes *': return shapes_printer(val)
    if str(val.type) == 'CSG_Shapes': return shapes_printer(val)
    if str(val.type) == 'CSG_Simple_Statistics': return simple_statistics_printer(val)
    if str(val.type) == 'CSG_Simple_Statistics &': return simple_statistics_printer(val)

gdb.pretty_printers.append(my_pp_func)
