import streamlit as st
import subprocess
import tempfile
import os
import re

# Page configuration
st.set_page_config(
    page_title="NetC Compiler",
    page_icon="🚀",
    layout="wide",
    initial_sidebar_state="expanded"
)

# Custom CSS
st.markdown("""
<style>
    .stTextArea textarea {
        font-family: 'Courier New', monospace;
        font-size: 14px;
    }
    .success-box {
        padding: 20px;
        background-color: #d4edda;
        border: 1px solid #c3e6cb;
        border-radius: 5px;
        color: #155724;
    }
    .error-box {
        padding: 20px;
        background-color: #f8d7da;
        border: 1px solid #f5c6cb;
        border-radius: 5px;
        color: #721c24;
    }
    .info-box {
        padding: 20px;
        background-color: #d1ecf1;
        border: 1px solid #bee5eb;
        border-radius: 5px;
        color: #0c5460;
    }
</style>
""", unsafe_allow_html=True)

# Initialize session state
if 'code' not in st.session_state:
    st.session_state.code = """# NetC Sample Program

link "math.ai";

network add(dnum a, dnum b)
{
    dnum result = a + b;
    yield result;
}

init()
{
    # Variable declarations
    dnum x = 5;
    dnum y = 10;
    text message = "Hello NetC";
    
    # Function call
    dnum sum = add(x, y);
    forward(sum);
    
    # Control structure
    if (x < y)
    {
        forward(message);
    }
}
"""

if 'compiled_output' not in st.session_state:
    st.session_state.compiled_output = None

# Sidebar
with st.sidebar:
    st.title("🚀 NetC Compiler")
    st.markdown("---")
    
    st.subheader("⚙️ Compiler Options")
    show_ast = st.checkbox("Show AST", value=True)
    show_symbols = st.checkbox("Show Symbol Table", value=True)
    
    st.markdown("---")
    
    st.subheader("📝 Examples")
    
    if st.button("Load Sample Code"):
        st.session_state.code = """# NetC Sample Program

link "math.ai";

network add(dnum a, dnum b)
{
    dnum result = a + b;
    yield result;
}

init()
{
    dnum x = 5;
    dnum y = 10;
    text message = "Hello NetC";
    
    dnum sum = add(x, y);
    forward(sum);
    
    if (x < y)
    {
        forward(message);
    }
}
"""
        st.rerun()
    
    if st.button("Load Error Example"):
        st.session_state.code = """# Code with semantic errors

init()
{
    # Error: undeclared variable
    x = 10;
    
    # Error: type mismatch
    dnum number = "text";
    
    # Error: redeclaration
    dnum y = 5;
    dnum y = 10;
}
"""
        st.rerun()
    
    if st.button("Clear Code"):
        st.session_state.code = ""
        st.rerun()
    
    st.markdown("---")
    
    st.subheader("ℹ️ About")
    st.markdown("""
    **NetC Compiler**
    
    A complete compiler for the NetC programming language with:
    - Lexical Analysis (Scanner)
    - Syntax Analysis (Parser)
    - Semantic Analysis
    
    Created for Compiler Design Course
    """)

# Main content
st.title("NetC Compiler")
st.markdown("Write your NetC code below and click **Compile** to analyze it.")

# Code editor
code = st.text_area(
    "Code Editor",
    value=st.session_state.code,
    height=400,
    key="code_editor",
    placeholder="Write your NetC code here..."
)

st.session_state.code = code

# Compile button
col1, col2, col3 = st.columns([1, 1, 4])
with col1:
    compile_btn = st.button("▶️ Compile", type="primary", use_container_width=True)
with col2:
    clear_btn = st.button("🗑️ Clear Output", use_container_width=True)

if clear_btn:
    st.session_state.compiled_output = None
    st.rerun()

# Compiler path - UPDATE THIS to match your executable location
compiler_path = "bin/netc.exe"  # or "netc.exe" depending on your file

def run_compiler(code_text, show_ast, show_symbols):
    """Run the NetC compiler and return output"""
    try:
        # Create temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.netc', delete=False) as temp_file:
            temp_file.write(code_text)
            temp_filename = temp_file.name
        
        # Build command
        cmd = [compiler_path, temp_filename]
        
        if show_ast:
            cmd.append("--show-ast")
        if show_symbols:
            cmd.append("--show-symbols")
        
        # Run compiler
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            cwd=os.getcwd()
        )
        
        # Clean up
        os.unlink(temp_filename)
        
        return {
            'stdout': result.stdout,
            'stderr': result.stderr,
            'returncode': result.returncode,
            'success': result.returncode == 0
        }
        
    except FileNotFoundError:
        return {
            'error': f"Compiler not found at: {compiler_path}",
            'success': False
        }
    except Exception as e:
        return {
            'error': str(e),
            'success': False
        }

def extract_tokens(output):
    """Extract tokens from compiler output"""
    match = re.search(
        r'====================== TOKEN LIST ======================\n(.*?)\n========================================================',
        output,
        re.DOTALL
    )
    if match:
        return match.group(1).strip()
    return None

def extract_ast(output):
    """Extract AST from compiler output"""
    if "Program" in output:
        ast_lines = []
        in_ast = False
        for line in output.split('\n'):
            if line.strip().startswith("Program"):
                in_ast = True
            if in_ast:
                if line.startswith("===") and "completed" in line:
                    break
                ast_lines.append(line)
        
        if ast_lines:
            return '\n'.join(ast_lines)
    return None

def extract_errors(output):
    """Extract errors and warnings from compiler output"""
    errors = []
    for line in output.split('\n'):
        if 'Error' in line or 'error' in line or 'Warning' in line:
            errors.append(line.strip())
    return errors

def extract_phase_info(output):
    """Extract information about which phase succeeded/failed"""
    phases = {
        'scanner': '❓',
        'parser': '❓',
        'semantic': '❓'
    }
    
    if "Scanning completed" in output:
        phases['scanner'] = '✅'
    elif "Scan" in output and "error" in output.lower():
        phases['scanner'] = '❌'
    
    if "Parsing completed successfully" in output:
        phases['parser'] = '✅'
    elif "Parsing" in output and "error" in output.lower():
        phases['parser'] = '❌'
    
    if "Semantic analysis completed successfully" in output:
        phases['semantic'] = '✅'
    elif "Semantic analysis completed with" in output and "error" in output:
        phases['semantic'] = '❌'
    
    return phases

# Compile button handler
if compile_btn:
    if not code.strip():
        st.warning("⚠️ Please enter some code to compile!")
    else:
        with st.spinner("Compiling..."):
            result = run_compiler(code, show_ast, show_symbols)
            st.session_state.compiled_output = result

# Display results
if st.session_state.compiled_output:
    result = st.session_state.compiled_output
    
    if 'error' in result:
        st.error(f"❌ {result['error']}")
    else:
        full_output = result['stdout'] + result['stderr']
        
        # Show compilation status
        if result['success']:
            st.markdown('<div class="success-box">✅ <b>Compilation Successful!</b><br>All phases completed without errors.</div>', unsafe_allow_html=True)
        else:
            st.markdown('<div class="error-box">❌ <b>Compilation Failed</b><br>Errors were found during compilation.</div>', unsafe_allow_html=True)
        
        st.markdown("---")
        
        # Phase status
        phases = extract_phase_info(full_output)
        st.subheader("📊 Compilation Phases")
        
        col1, col2, col3 = st.columns(3)
        with col1:
            st.metric("Scanner", phases['scanner'])
        with col2:
            st.metric("Parser", phases['parser'])
        with col3:
            st.metric("Semantic", phases['semantic'])
        
        st.markdown("---")
        
        # Tabs for different outputs
        tab1, tab2, tab3, tab4 = st.tabs(["📋 Console", "🔤 Tokens", "🌲 AST", "❗ Errors"])
        
        with tab1:
            st.subheader("Console Output")
            st.code(full_output, language="text")
        
        with tab2:
            st.subheader("Token List")
            tokens = extract_tokens(full_output)
            if tokens:
                st.code(tokens, language="text")
                
                # Count tokens
                token_lines = [l for l in tokens.split('\n') if l.strip() and not l.startswith('-')]
                st.info(f"📊 Total tokens: {len(token_lines)}")
            else:
                st.warning("No tokens found in output")
        
        with tab3:
            st.subheader("Abstract Syntax Tree")
            ast = extract_ast(full_output)
            if ast:
                st.code(ast, language="text")
            else:
                st.info("AST not displayed. Enable 'Show AST' option to see the tree structure.")
        
        with tab4:
            st.subheader("Errors and Warnings")
            errors = extract_errors(full_output)
            if errors:
                for error in errors:
                    if "Error" in error:
                        st.error(error)
                    elif "Warning" in error:
                        st.warning(error)
                
                st.markdown(f"**Found {len(errors)} issue(s)**")
            else:
                st.success("✅ No errors or warnings!")

# Footer
st.markdown("---")
st.markdown("""
<div style='text-align: center; color: gray;'>
    <small>NetC Compiler | Built with Streamlit | Compiler Design Project</small>
</div>
""", unsafe_allow_html=True)