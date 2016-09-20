/* CirKit: A circuit toolkit
 * Copyright (C) 2009-2015  University of Bremen
 * Copyright (C) 2015-2016  EPFL
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "read_qc.hpp"

#include <unordered_map>
#include <vector>

#include <boost/regex.hpp>

#include <core/utils/range_utils.hpp>
#include <core/utils/string_utils.hpp>
#include <reversible/gate.hpp>
#include <reversible/functions/add_gates.hpp>

namespace cirkit
{

circuit read_qc( const std::string& filename )
{
  circuit circ;
  std::unordered_map<std::string, unsigned> var2line;

  line_parser( filename, {
      {boost::regex( "^\\.v *(.*)$" ), [&circ, &var2line]( const boost::smatch& m ) {
          std::vector<std::string> variables;
          split_string( variables, m[1u], " " );

          circ.set_lines( variables.size() );
          for ( const auto& v : index( variables ) )
          {
            var2line[v.value + "'"] = v.index;
            var2line[v.value] = v.index;
          }
        }},
      {boost::regex( "^t(\\d+) *(.*)$" ), [&circ, &var2line]( const boost::smatch& m ) {
          std::vector<std::string> lines;
          split_string( lines, m[2u], " " );

          assert( boost::lexical_cast<unsigned>( std::string( m[1u] ) ) == lines.size() );

          gate::control_container controls;
          for ( auto i = 0; i < lines.size() - 1; ++i )
          {
            controls.push_back( make_var( var2line[lines[i]], lines[i].back() != '\'' ) );
          }
          append_toffoli( circ, controls, var2line[lines.back()] );
        }}
    } );

  return circ;
}

}

// Local Variables:
// c-basic-offset: 2
// eval: (c-set-offset 'substatement-open 0)
// eval: (c-set-offset 'innamespace 0)
// End:
