"""Count compilation attempts

Revision ID: 4a9f1c7e5495
Revises: 8cf76fe04ac8
Create Date: 2018-10-22 17:17:00.884182+00:00

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import postgresql


# revision identifiers, used by Alembic.
revision = '4a9f1c7e5495'
down_revision = '8cf76fe04ac8'
branch_labels = None
depends_on = None


def upgrade():
    op.add_column('bot',
        sa.Column('compile_attempts', sa.Integer(), server_default='0'))


def downgrade():
    op.drop_column('bot', 'compile_attempts')
